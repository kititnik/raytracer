#!/usr/bin/env python3
"""Run raytracer benchmarks through hyperfine and compare results.

    python3 bench/run.py run <name>            build, benchmark, check images
    python3 bench/run.py compare <old> <new>   compare two saved runs

Results are written to bench/results/<name>/.
"""

import argparse
import json
import shlex
import statistics
import subprocess
import sys
import tomllib
from datetime import datetime
from pathlib import Path

from PIL import Image, ImageChops

ROOT = Path(__file__).resolve().parent.parent
BENCH_DIR = ROOT / "bench"
RESULTS_DIR = BENCH_DIR / "results"
SCENES_FILE = BENCH_DIR / "scenes.toml"
BUILD_DIR = ROOT / "build"
BINARY = BUILD_DIR / "raytracer"
DEFAULT_REFERENCE = "baseline"
PHASES = ("parse_ms", "render_ms", "write_ms")


class BenchError(Exception):
    pass


# --- run ---------------------------------------------------------------------

def load_config():
    with SCENES_FILE.open("rb") as f:
        config = tomllib.load(f)
    if not config.get("scene"):
        raise BenchError(f"No [[scene]] entries in {SCENES_FILE}")
    return config


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, check=True,
                          capture_output=True, text=True).stdout.strip()


def read_cmake_cache():
    cache_file = BUILD_DIR / "CMakeCache.txt"
    if not cache_file.exists():
        raise BenchError(f"{cache_file} not found, configure the project first")
    cache = {}
    for line in cache_file.read_text().splitlines():
        if line.startswith(("#", "//")) or "=" not in line:
            continue
        key, value = line.split("=", 1)
        cache[key.split(":", 1)[0]] = value
    return cache


def collect_meta(cache, dirty, args):
    compiler = cache.get("CMAKE_CXX_COMPILER", "")
    compiler_version = subprocess.run([compiler, "--version"], capture_output=True,
                                      text=True).stdout.splitlines()[0] if compiler else ""
    hyperfine_version = subprocess.run(["hyperfine", "--version"], capture_output=True,
                                       text=True).stdout.strip()
    build_type = cache.get("CMAKE_BUILD_TYPE", "")
    return {
        "commit": git("rev-parse", "HEAD"),
        "dirty": dirty,
        "build_type": build_type,
        "cxx_flags": cache.get("CMAKE_CXX_FLAGS", ""),
        "cxx_flags_build_type": cache.get(f"CMAKE_CXX_FLAGS_{build_type.upper()}", ""),
        "compiler": compiler_version,
        "hyperfine": hyperfine_version,
        "shell": "none",
        "runs": args.runs,
        "warmup": args.warmup,
        "date": datetime.now().astimezone().isoformat(timespec="seconds"),
    }


def build_command(scene, out_dir, timings_dir):
    name = scene["name"]
    image = out_dir / f"{name}.png" if scene.get("check_image", True) else Path("/dev/null")
    args = [
        str(BINARY),
        "--input", scene["input"],
        "--output", str(image),
        "--width", str(scene["width"]),
        "--height", str(scene["height"]),
    ]
    if "mode" in scene:
        args += ["--mode", scene["mode"]]
    if "depth" in scene:
        args += ["--depth", str(scene["depth"])]
    if "look_from" in scene:
        args += ["--look-from", scene["look_from"]]
    if "look_to" in scene:
        args += ["--look-to", scene["look_to"]]
    args += ["--timings", str(timings_dir / f"{name}.jsonl")]
    return shlex.join(args)


def run_hyperfine(scenes, out_dir, timings_dir, args):
    cmd = [
        "hyperfine",
        "--shell=none",
        "--warmup", str(args.warmup),
        "--runs", str(args.runs),
        "--export-json", str(out_dir / "results.json"),
        "--export-markdown", str(out_dir / "results.md"),
    ]
    for scene in scenes:
        cmd += ["-n", scene["name"], build_command(scene, out_dir, timings_dir)]
    subprocess.run(cmd, cwd=ROOT, check=True)


def aggregate_phases(scenes, timings_dir, args):
    phases = {}
    for scene in scenes:
        name = scene["name"]
        lines = (timings_dir / f"{name}.jsonl").read_text().splitlines()
        if len(lines) != args.warmup + args.runs:
            raise BenchError(f"Expected {args.warmup + args.runs} timing lines for {name}, "
                             f"got {len(lines)}")
        # hyperfine runs all warmups of a command before its measured runs.
        samples = [json.loads(line) for line in lines[args.warmup:]]
        phases[name] = {key: statistics.median(s[key] for s in samples) for key in PHASES}
    return phases


def compare_images(new_path, ref_path, tolerance, diff_path):
    """Returns (ok, message)."""
    new = Image.open(new_path).convert("RGB")
    ref = Image.open(ref_path).convert("RGB")
    if new.size != ref.size:
        return False, f"size {new.size} != reference {ref.size}"

    r, g, b = ImageChops.difference(new, ref).split()
    per_pixel_max = ImageChops.lighter(ImageChops.lighter(r, g), b)
    histogram = per_pixel_max.histogram()

    channel = tolerance["channel"]
    total = new.width * new.height
    bad = sum(histogram[channel + 1:])
    max_diff = max(value for value, count in enumerate(histogram) if count)
    allowed = int(total * tolerance["max_bad_fraction"])
    message = f"max diff {max_diff}, {bad} px > {channel} (allowed {allowed})"

    if bad > allowed:
        per_pixel_max.point(lambda v: 255 if v > channel else 0).save(diff_path)
        return False, f"{message}, see {diff_path.relative_to(ROOT)}"
    return True, message


def check_images(scenes, out_dir, ref_dir, tolerance):
    print(f"\nImages vs {ref_dir.relative_to(ROOT)}:")
    ok = True
    for scene in scenes:
        if not scene.get("check_image", True):
            continue
        name = scene["name"]
        ref_path = ref_dir / f"{name}.png"
        if not ref_path.exists():
            print(f"  {name:<20} SKIP  no reference image")
            continue
        passed, message = compare_images(out_dir / f"{name}.png", ref_path, tolerance,
                                         out_dir / f"{name}.diff.png")
        print(f"  {name:<20} {'OK  ' if passed else 'FAIL'}  {message}")
        ok = ok and passed
    return ok


def cmd_run(args):
    config = load_config()
    scenes = config["scene"]
    out_dir = RESULTS_DIR / args.name
    if out_dir.exists() and not args.force:
        raise BenchError(f"{out_dir.relative_to(ROOT)} already exists, use --force to overwrite")

    dirty = bool(git("status", "--porcelain", "--untracked-files=no"))
    if dirty and not args.allow_dirty:
        raise BenchError("Uncommitted changes: commit them or pass --allow-dirty")

    subprocess.run(["cmake", "--build", str(BUILD_DIR)], cwd=ROOT, check=True)
    cache = read_cmake_cache()
    if cache.get("CMAKE_BUILD_TYPE") != "Release":
        raise BenchError(f"Build type is '{cache.get('CMAKE_BUILD_TYPE', '')}', expected Release. "
                         f"Run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release")

    timings_dir = out_dir / "timings"
    timings_dir.mkdir(parents=True, exist_ok=True)
    for old in timings_dir.glob("*.jsonl"):
        old.unlink()

    meta = collect_meta(cache, dirty, args)
    (out_dir / "meta.json").write_text(json.dumps(meta, indent=2) + "\n")

    run_hyperfine(scenes, out_dir, timings_dir, args)

    phases = aggregate_phases(scenes, timings_dir, args)
    (out_dir / "phases.json").write_text(json.dumps(phases, indent=2) + "\n")
    print("\nPhases (median, ms):")
    for name, values in phases.items():
        print(f"  {name:<20} " + "  ".join(f"{k.removesuffix('_ms')} {v:9.3f}"
                                            for k, v in values.items()))

    ref_dir = RESULTS_DIR / (args.reference or DEFAULT_REFERENCE)
    if ref_dir == out_dir:
        return 0
    if not ref_dir.exists():
        print(f"\nNo reference run {ref_dir.relative_to(ROOT)}, images not checked")
        return 0
    return 0 if check_images(scenes, out_dir, ref_dir, config["tolerance"]) else 1


# --- compare -----------------------------------------------------------------

def load_run(name):
    run_dir = RESULTS_DIR / name
    results_file = run_dir / "results.json"
    if not results_file.exists():
        raise BenchError(f"{results_file.relative_to(ROOT)} not found")
    results = json.loads(results_file.read_text())["results"]
    summaries = {r["name"]: r["summary"] for r in results}

    phases_file = run_dir / "phases.json"
    phases = json.loads(phases_file.read_text()) if phases_file.exists() else {}

    meta_file = run_dir / "meta.json"
    meta = json.loads(meta_file.read_text()) if meta_file.exists() else None
    return summaries, phases, meta


def change(old, new):
    return f"{(new - old) / old * 100:+6.1f}%" if old else "    n/a"


def print_row(label, old, new, unit):
    print(f"  {label:<22} {old:>12.1f} {unit:<2} {new:>12.1f} {unit:<2} {change(old, new)}")


def warn_meta_mismatch(old_meta, new_meta):
    if old_meta is None or new_meta is None:
        print("warning: meta.json missing in one of the runs, build settings not compared")
        return
    for key in ("build_type", "cxx_flags", "cxx_flags_build_type", "compiler", "shell"):
        if old_meta.get(key) != new_meta.get(key):
            print(f"warning: {key} differs: '{old_meta.get(key)}' vs '{new_meta.get(key)}'")
    for name, meta in (("old", old_meta), ("new", new_meta)):
        if meta.get("dirty"):
            print(f"warning: {name} run was made with uncommitted changes")


def cmd_compare(args):
    old_summaries, old_phases, old_meta = load_run(args.old)
    new_summaries, new_phases, new_meta = load_run(args.new)
    warn_meta_mismatch(old_meta, new_meta)

    print(f"\n  {'':<22} {args.old:>15} {args.new:>15}  change")
    for name in old_summaries:
        if name not in new_summaries:
            print(f"{name}: missing in {args.new}")
            continue
        old, new = old_summaries[name], new_summaries[name]
        print(name)
        print_row("wall (median)", old["time_wall_clock"]["median"] * 1000,
                  new["time_wall_clock"]["median"] * 1000, "ms")
        if "instructions" in old and "instructions" in new:
            print_row("instructions (median)", old["instructions"]["median"] / 1e6,
                      new["instructions"]["median"] / 1e6, "M")
        if name in old_phases and name in new_phases:
            for phase in PHASES:
                print_row(phase.removesuffix("_ms"), old_phases[name][phase],
                          new_phases[name][phase], "ms")
    return 0


# --- main --------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    run = sub.add_parser("run", help="build, benchmark and check images")
    run.add_argument("name", help="result directory name in bench/results/")
    run.add_argument("--runs", type=int, default=10)
    run.add_argument("--warmup", type=int, default=1)
    run.add_argument("--reference", help=f"run to check images against "
                                         f"(default: {DEFAULT_REFERENCE})")
    run.add_argument("--allow-dirty", action="store_true",
                     help="allow benchmarking with uncommitted changes")
    run.add_argument("--force", action="store_true", help="overwrite an existing run")
    run.set_defaults(func=cmd_run)

    compare = sub.add_parser("compare", help="compare two runs")
    compare.add_argument("old")
    compare.add_argument("new")
    compare.set_defaults(func=cmd_compare)

    args = parser.parse_args()
    try:
        return args.func(args)
    except BenchError as e:
        print(f"error: {e}", file=sys.stderr)
        return 1
    except subprocess.CalledProcessError as e:
        print(f"error: command failed with exit code {e.returncode}: {shlex.join(e.cmd)}",
              file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())

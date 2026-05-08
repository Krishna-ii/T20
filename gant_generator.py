#!/usr/bin/env python3

import csv
import os
import sys
from collections import defaultdict

CSV_PATH = "data/gantt_log.csv"
OUT_PATH = "GANTT_CHART.md"

INDIA_BATSMEN = {
    0: "Rohit Sharma", 1: "Virat Kohli", 2: "Shubman Gill",
    3: "KL Rahul", 4: "Hardik Pandya", 5: "Ravindra Jadeja",
    6: "Axar Patel", 7: "Kuldeep Yadav", 8: "Jasprit Bumrah",
    9: "Mohammed Shami", 10: "Arshdeep Singh",
}

AUSTRALIA_BATSMEN = {
    0: "David Warner", 1: "Travis Head", 2: "Steve Smith",
    3: "Glenn Maxwell", 4: "Marnus Labuschagne", 5: "Marcus Stoinis",
    6: "Matthew Wade", 7: "Pat Cummins", 8: "Mitchell Starc",
    9: "Josh Hazlewood", 10: "Adam Zampa",
}

# ✅ ADDED: opener exclusion
INDIA_OPENERS = {"Rohit Sharma", "Virat Kohli"}
AUSTRALIA_OPENERS = {"David Warner", "Travis Head"}

INDIA_BOWLERS = {
    0: "Jasprit Bumrah", 1: "Mohammed Shami", 2: "Arshdeep Singh",
    3: "Hardik Pandya", 4: "Kuldeep Yadav",
}

AUSTRALIA_BOWLERS = {
    0: "Pat Cummins", 1: "Mitchell Starc", 2: "Josh Hazlewood",
    3: "Adam Zampa", 4: "Marcus Stoinis",
}

TEAM_NAMES = {0: "India", 1: "Australia"}

EVENT_EMOJI = {
    "DOT": ".", "SINGLE": "1", "DOUBLE": "2", "FOUR": "4",
    "SIX": "6", "WIDE": "W", "NO_BALL": "N", "WICKET": "X",
}

RUN_MAP = {
    "SINGLE": 1, "DOUBLE": 2, "FOUR": 4,
    "SIX": 6, "WIDE": 1, "NO_BALL": 1,
}


def batsman_name(idx, batting_team):
    if batting_team == 0:
        return INDIA_BATSMEN.get(idx, f"Bat-{idx}")
    return AUSTRALIA_BATSMEN.get(idx, f"Bat-{idx}")


def bowler_name(idx, batting_team):
    if batting_team == 0:
        return AUSTRALIA_BOWLERS.get(idx, f"Bowler-{idx}")
    return INDIA_BOWLERS.get(idx, f"Bowler-{idx}")


def event_symbol(ev):
    return EVENT_EMOJI.get(ev.upper(), ev[0] if ev else "?")


def load_csv(path):
    rows = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append({
                "innings": int(row["innings"]),
                "batting_team": int(row["batting_team"]),
                "ball": int(row["ball_number"]),
                "over": int(row["over"]),
                "ball_in_over": int(row["ball_in_over"]),
                "bowler": int(row["bowler"]),
                "striker": int(row["striker"]),
                "event": row["event"].strip(),
                "score": int(row["score_after"]),
                "wickets": int(row["wickets_after"]),
                "ts_ms": int(row["timestamp_ms"]),
            })
    return rows


def build_header(rows):
    innings_nums = sorted(set(r["innings"] for r in rows))
    lines = [
        "# T20 Cricket Simulator — Gantt Chart",
        "",
        "| Metric | Value |",
        "|--------|-------|",
        f"| Total deliveries logged | {len(rows)} |",
        f"| Innings played | {len(innings_nums)} |",
    ]
    for inn in innings_nums:
        inn_rows = [r for r in rows if r["innings"] == inn]
        last = inn_rows[-1]
        bat_label = TEAM_NAMES[inn_rows[0]["batting_team"]]
        lines.append(
            f"| Innings {inn} ({bat_label}) final score "
            f"| {last['score']} / {last['wickets']} |"
        )
    lines += ["", "---", ""]
    return "\n".join(lines)


def build_bowler_gantt(rows):
    innings_groups = defaultdict(list)
    for r in rows:
        innings_groups[r["innings"]].append(r)

    lines = [
        "## Bowler Gantt — Pitch Ownership per Ball", "",
    ]

    for inn_num in sorted(innings_groups):
        inn_rows = innings_groups[inn_num]
        batting_team = inn_rows[0]["batting_team"]
        bat_label = TEAM_NAMES[batting_team]
        bowl_label = TEAM_NAMES[1 - batting_team]

        lines.append(
            f"### Innings {inn_num} — {bat_label} batting | {bowl_label} bowling"
        )
        lines.append("")

        over_groups = defaultdict(list)
        for r in inn_rows:
            over_groups[r["over"]].append(r)

        for over_num, over_rows in sorted(over_groups.items()):
            bowler_id = over_rows[0]["bowler"]
            blabel = bowler_name(bowler_id, batting_team)

            lines.append(f"#### Over {over_num + 1} — {blabel}")
            lines.append("")

            col_headers = ["| | "]
            sep = ["|---|"]
            for r in over_rows:
                col_headers.append(f" Ball {r['ball_in_over']} |")
                sep.append(":---:|")
            lines.append("".join(col_headers))
            lines.append("".join(sep))

            br = [f"| {blabel} |"]
            for r in over_rows:
                br.append(f" {event_symbol(r['event'])} |")
            lines.append("".join(br))

            er = ["| Event |"]
            for r in over_rows:
                er.append(f" `{r['event']}` |")
            lines.append("".join(er))

            sr = ["| Score after |"]
            for r in over_rows:
                sr.append(f" {r['score']}/{r['wickets']} |")
            lines.append("".join(sr))
            lines.append("")

    lines += ["---", ""]
    return "\n".join(lines)


def build_batsman_gantt(rows):
    innings_groups = defaultdict(list)
    for r in rows:
        innings_groups[r["innings"]].append(r)

    lines = [
        "## Batsman Gantt — Crease Occupancy per Ball", "",
    ]

    for inn_num in sorted(innings_groups):
        inn_rows = innings_groups[inn_num]
        batting_team = inn_rows[0]["batting_team"]
        bat_label = TEAM_NAMES[batting_team]

        lines.append(f"### Innings {inn_num} — {bat_label} batting")
        lines.append("")

        over_groups = defaultdict(list)
        for r in inn_rows:
            over_groups[r["over"]].append(r)

        for over_num, over_rows in sorted(over_groups.items()):
            lines.append(f"#### Over {over_num + 1}")
            lines.append("")

            col_headers = ["| Thread / Resource |"]
            sep = ["|:------------------|"]
            for r in over_rows:
                col_headers.append(f" Ball {r['ball_in_over']} |")
                sep.append(":------:|")
            lines.append("".join(col_headers))
            lines.append("".join(sep))

            for bat_idx in sorted(set(r["striker"] for r in over_rows)):
                bname = batsman_name(bat_idx, batting_team)
                rc = [f"| {bname} |"]
                for r in over_rows:
                    if r["striker"] == bat_idx:
                        rc.append(f" {event_symbol(r['event'])} |")
                    else:
                        rc.append(" . |")
                lines.append("".join(rc))

            lines.append("")

    lines += ["---", ""]
    return "\n".join(lines)


def build_wait_time_analysis(rows):
    innings_groups = defaultdict(list)
    for r in rows:
        innings_groups[r["innings"]].append(r)

    lines = [
        "## Batsman Waiting Time Analysis (SJF vs FCFS)",
        "",
        "_Middle order only — excludes configured openers._",
        "",
    ]

    innings_detail = {}
    overall_total = 0
    overall_count = 0

    for inn_num in sorted(innings_groups):
        inn_rows = innings_groups[inn_num]
        batting_team = inn_rows[0]["batting_team"]

        opener_set = INDIA_OPENERS if batting_team == 0 else AUSTRALIA_OPENERS

        first_ball = {}
        for r in inn_rows:
            s = r["striker"]
            if s not in first_ball:
                first_ball[s] = r["ball"]

        batters_in_order = sorted(first_ball.keys(), key=lambda x: first_ball[x])

        detail = []
        total_w = 0
        count = 0

        for bat_idx in batters_in_order:
            bname = batsman_name(bat_idx, batting_team)

            # ✅ skip openers
            if bname in opener_set:
                continue

            first_ball_no = first_ball[bat_idx]
            wait_balls = first_ball_no

            total_w += wait_balls
            count += 1
            detail.append({
                "bat_idx": bat_idx,
                "first_ball_no": first_ball_no,
                "wait_balls": wait_balls,
            })

        avg = total_w / count if count else 0.0
        innings_detail[inn_num] = {
            "batting_team": batting_team,
            "detail": detail,
            "total": total_w,
            "count": count,
            "avg": avg,
        }
        overall_total += total_w
        overall_count += count

    lines.append("### Summary")
    lines.append("")
    lines.append(
        "| Innings | Batting Team | Batsmen | Total Wait (balls) | Avg Wait (balls) |"
    )
    lines.append(
        "|:-------:|:------------:|:-------:|:------------------:|:----------------:|"
    )

    for inn_num in sorted(innings_detail):
        d = innings_detail[inn_num]
        bat_label = TEAM_NAMES[d["batting_team"]]
        lines.append(
            f"| {inn_num} | {bat_label} | {d['count']} "
            f"| {d['total']} | {d['avg']:.2f} |"
        )

    overall_avg = overall_total / overall_count if overall_count else 0.0
    lines.append(
        f"| Both | — | {overall_count} | {overall_total} | {overall_avg:.2f} |"
    )
    lines.append("")

    for inn_num in sorted(innings_detail):
        d = innings_detail[inn_num]
        batting_team = d["batting_team"]
        bat_label = TEAM_NAMES[batting_team]

        lines.append(f"### Innings {inn_num} — {bat_label} batting")
        lines.append("")
        lines.append(
            "| # | Batsman | Arrival | First Ball | Wait |"
        )
        lines.append(
            "|:-:|---------|:-------:|:----------:|:----:|"
        )

        for rank, entry in enumerate(d["detail"], 1):
            bname = batsman_name(entry["bat_idx"], batting_team)
            lines.append(
                f"| {rank} | {bname} | 0 "
                f"| {entry['first_ball_no']} "
                f"| {entry['wait_balls']} |"
            )

        lines.append(f"| | Average | | | {d['avg']:.2f} |")
        lines.append("")

    lines += ["---", ""]
    return "\n".join(lines)


def build_ball_log(rows):
    innings_groups = defaultdict(list)
    for r in rows:
        innings_groups[r["innings"]].append(r)

    lines = ["## Ball-by-Ball Event Log", ""]

    for inn_num in sorted(innings_groups):
        inn_rows = innings_groups[inn_num]
        batting_team = inn_rows[0]["batting_team"]
        bat_label = TEAM_NAMES[batting_team]

        lines.append(f"### Innings {inn_num} — {bat_label} batting")
        lines.append("")
        lines.append(
            "| Ball # | Over | Ball | Bowler | Striker | Event | Score | Wkts |"
        )
        lines.append(
            "|:------:|:----:|:----:|--------|---------|:-----:|:-----:|:----:|"
        )

        for r in inn_rows:
            lines.append(
                f"| {r['ball']} | {r['over']+1} | {r['ball_in_over']} "
                f"| {bowler_name(r['bowler'], batting_team)} "
                f"| {batsman_name(r['striker'], batting_team)} "
                f"| `{r['event']}` | {r['score']} | {r['wickets']} |"
            )
        lines.append("")

    lines += ["---", ""]
    return "\n".join(lines)


def main():
    if not os.path.exists(CSV_PATH):
        print(f"ERROR: {CSV_PATH} not found.", file=sys.stderr)
        sys.exit(1)

    rows = load_csv(CSV_PATH)
    if not rows:
        print("ERROR: gantt_log.csv is empty.", file=sys.stderr)
        sys.exit(1)

    innings_found = sorted(set(r["innings"] for r in rows))
    print(f"Loaded {len(rows)} deliveries across innings: {innings_found}")

    md = "\n".join([
        build_header(rows),
        build_bowler_gantt(rows),
        build_batsman_gantt(rows),
        build_wait_time_analysis(rows),
        build_ball_log(rows),
        "_Generated automatically by gant_generator.py_",
    ])

    with open(OUT_PATH, "w") as f:
        f.write(md)

    print(f"Gantt chart written → {OUT_PATH}")


if __name__ == "__main__":
    main()
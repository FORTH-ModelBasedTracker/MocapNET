import os
import json
from collections import defaultdict

#To count frames on a video..
#ffprobe -v error -count_frames -select_streams v:0 -show_entries stream=nb_read_frames -of default=nokey=1:noprint_wrappers=1 video.avi

SKIP_DIRS = {"allTrackingResults"}
root_dirs = ["/media/ammar/games2/Datasets/test"]
confidence_threshold = 0.5

BODY_25_NAMES = {
    0:  "Nose",
    1:  "Neck",
    2:  "RShoulder",
    3:  "RElbow",
    4:  "RWrist",
    5:  "LShoulder",
    6:  "LElbow",
    7:  "LWrist",
    8:  "MidHip",
    9:  "RHip",
    10: "RKnee",
    11: "RAnkle",
    12: "LHip",
    13: "LKnee",
    14: "LAnkle",
    15: "REye",
    16: "LEye",
    17: "REar",
    18: "LEar",
    19: "LBigToe",
    20: "LSmallToe",
    21: "LHeel",
    22: "RBigToe",
    23: "RSmallToe",
    24: "RHeel",
}

HAND_21_NAMES = {
    0:  "Wrist",
    1:  "Thumb1",
    2:  "Thumb2",
    3:  "Thumb3",
    4:  "Thumb4",
    5:  "Index1",
    6:  "Index2",
    7:  "Index3",
    8:  "Index4",
    9:  "Middle1",
    10: "Middle2",
    11: "Middle3",
    12: "Middle4",
    13: "Ring1",
    14: "Ring2",
    15: "Ring3",
    16: "Ring4",
    17: "Pinky1",
    18: "Pinky2",
    19: "Pinky3",
    20: "Pinky4",
}

stats = {
    "pose_keypoints_2d": {
        "missing_counts": defaultdict(int),
        "low_conf_counts": defaultdict(int),
        "total_joints": 0,
        "missing_joints": 0,
        "low_conf_joints": 0,
        "frames_with_missing": 0,
        "frames_with_low_conf": 0,
    },
    "hand_left_keypoints_2d": {
        "missing_counts": defaultdict(int),
        "low_conf_counts": defaultdict(int),
        "total_joints": 0,
        "missing_joints": 0,
        "low_conf_joints": 0,
        "frames_with_missing": 0,
        "frames_with_low_conf": 0,
    },
    "hand_right_keypoints_2d": {
        "missing_counts": defaultdict(int),
        "low_conf_counts": defaultdict(int),
        "total_joints": 0,
        "missing_joints": 0,
        "low_conf_joints": 0,
        "frames_with_missing": 0,
        "frames_with_low_conf": 0,
    },
}

total_frames = 0
frames_no_people = 0


def process_keypoints(arr, bucket, conf_thresh):
    frame_missing_local = False
    frame_low_conf_local = False

    for j in range(0, len(arr), 3):
        x = arr[j]
        y = arr[j + 1]
        c = arr[j + 2]
        joint_id = j // 3

        stats[bucket]["total_joints"] += 1

        is_missing = (c == 0) or (x == 0 and y == 0)
        is_low_conf = (not is_missing) and (c < conf_thresh)

        if is_missing:
            stats[bucket]["missing_counts"][joint_id] += 1
            stats[bucket]["missing_joints"] += 1
            frame_missing_local = True

        if is_low_conf:
            stats[bucket]["low_conf_counts"][joint_id] += 1
            stats[bucket]["low_conf_joints"] += 1
            frame_low_conf_local = True

    if frame_missing_local:
        stats[bucket]["frames_with_missing"] += 1

    if frame_low_conf_local:
        stats[bucket]["frames_with_low_conf"] += 1


for root_dir in root_dirs:
  for root, dirs, files in os.walk(root_dir, topdown=True):
    dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
 
    print("VISITING:", root)

    for f in files:
        if not f.endswith("_keypoints.json"):
            continue

        path = os.path.join(root, f)

        try:
            with open(path, "r") as jf:
                data = json.load(jf)
        except Exception as e:
            print(f"Could not read {path}: {e}")
            continue

        total_frames += 1

        if not data.get("people"):
            frames_no_people += 1
            continue

        person = data["people"][0]

        for bucket in [
            "pose_keypoints_2d",
            "hand_left_keypoints_2d",
            "hand_right_keypoints_2d",
        ]:
            arr = person.get(bucket, [])
            if arr:
                process_keypoints(arr, bucket, confidence_threshold)


print("========== OVERALL ==========")
print("Confidence threshold:", confidence_threshold)
print("Total frames:", total_frames)
print("Frames with no detected people:", frames_no_people)

for bucket in [
    "pose_keypoints_2d",
    "hand_left_keypoints_2d",
    "hand_right_keypoints_2d",
]:
    print("\n==========", bucket, "==========")

    total_joints = stats[bucket]["total_joints"]
    missing_joints = stats[bucket]["missing_joints"]
    low_conf_joints = stats[bucket]["low_conf_joints"]

    print("Frames with at least one missing joint:", stats[bucket]["frames_with_missing"])
    if total_frames > 0:
        print("Percent frames with missing joint: {:.2f}%".format(
            100.0 * stats[bucket]["frames_with_missing"] / total_frames
        ))

    print("Frames with at least one low-confidence joint:", stats[bucket]["frames_with_low_conf"])
    if total_frames > 0:
        print("Percent frames with low-confidence joint: {:.2f}%".format(
            100.0 * stats[bucket]["frames_with_low_conf"] / total_frames
        ))

    print("Total joints checked:", total_joints)

    print("Missing joints:", missing_joints)
    if total_joints > 0:
        print("Missing %: {:.2f}%".format(100.0 * missing_joints / total_joints))

    print("Low-confidence joints (< {:.3f}): {}".format(confidence_threshold, low_conf_joints))
    if total_joints > 0:
        print("Low-confidence %: {:.2f}%".format(100.0 * low_conf_joints / total_joints))

    print("\nMissing joints per joint id:")
    for joint_id in sorted(stats[bucket]["missing_counts"]):
        count = stats[bucket]["missing_counts"][joint_id]
        if bucket == "pose_keypoints_2d":
            name = BODY_25_NAMES.get(joint_id, f"Joint{joint_id}")
            print(f"  Joint {joint_id:2d} ({name:10s}) : {count}")
        else:
            name = HAND_21_NAMES.get(joint_id, f"Joint{joint_id}")
            print(f"  Joint {joint_id:2d} ({name:8s}) : {count}")

    print("\nLow-confidence joints per joint id:")
    for joint_id in sorted(stats[bucket]["low_conf_counts"]):
        count = stats[bucket]["low_conf_counts"][joint_id]
        if bucket == "pose_keypoints_2d":
            name = BODY_25_NAMES.get(joint_id, f"Joint{joint_id}")
            print(f"  Joint {joint_id:2d} ({name:10s}) : {count}")
        else:
            name = HAND_21_NAMES.get(joint_id, f"Joint{joint_id}")
            print(f"  Joint {joint_id:2d} ({name:8s}) : {count}")

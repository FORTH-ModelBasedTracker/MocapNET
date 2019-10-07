#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

../../GroundTruthDumper --from waveleft.bvh --onlyAnimateGivenJoints 2 lshoulder lelbow --bvh waveleft.bvh
../../GroundTruthDumper --from waveright.bvh --onlyAnimateGivenJoints 2 rshoulder relbow --bvh waveright.bvh
../../GroundTruthDumper --from comeleft.bvh --onlyAnimateGivenJoints 2 lshoulder lelbow --bvh comeleft.bvh
../../GroundTruthDumper --from comeright.bvh --onlyAnimateGivenJoints 2 rshoulder relbow --bvh comeright.bvh
../../GroundTruthDumper --from lefthandcircle.bvh --onlyAnimateGivenJoints 2 lshoulder lelbow --bvh lefthandcircle.bvh
../../GroundTruthDumper --from righthandcircle.bvh --onlyAnimateGivenJoints 2 rshoulder relbow --bvh righthandcircle.bvh
../../GroundTruthDumper --from help.bvh --onlyAnimateGivenJoints 4 lshoulder lelbow rshoulder relbow --bvh help.bvh
../../GroundTruthDumper --from push.bvh --onlyAnimateGivenJoints 4 lshoulder lelbow rshoulder relbow --bvh push.bvh
../../GroundTruthDumper --from doubleclap.bvh --onlyAnimateGivenJoints 4 lshoulder lelbow rshoulder relbow --bvh doubleclap.bvh
../../GroundTruthDumper --from tpose.bvh --onlyAnimateGivenJoints 4 lshoulder lelbow rshoulder relbow --bvh tpose.bvh
../../GroundTruthDumper --from leftkick.bvh --onlyAnimateGivenJoints 2 lhip lknee --bvh leftkick.bvh
../../GroundTruthDumper --from rightkick.bvh --onlyAnimateGivenJoints 2 rhip rknee --bvh rightkick.bvh

exit 0

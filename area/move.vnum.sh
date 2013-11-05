#!/bin/bash

VNUM_OLD=40884
VNUM_NEW=2497

# WEAR locations

WEAR_ABOUT=12
WEAR_ARMS=10
WEAR_BODY=5
WEAR_DUALWIELD=19
WEAR_EAR_L=21
WEAR_EAR_R=22
WEAR_FEET=8
WEAR_FINGER_L=1
WEAR_FINGER_R=2
WEAR_HANDS=9
WEAR_HEAD=6
WEAR_HOLD=17
WEAR_LEGS=7
WEAR_NECK_1=3
WEAR_NECK_2=4
WEAR_SHIIELD=11
WEAR_WAIST=13
WEAR_WIELD=16

WEAR=${WEAR_HOLD}

# poprawka wield -> wear dla kilofów
#perl -pi -e 's/E 100 '${VNUM_OLD}' 0 16/E 100 '${VNUM_NEW}' 0 17/g' *are

perl -pi -e 's/E 100 '${VNUM_OLD}' 0 '${WEAR}'/E 100 '${VNUM_NEW}' 0 '${WEAR}'/g' *are

# WEAR plus
#perl -pi -e 's/E 100 '${VNUM_OLD}' 0 '${WEAR_EAR_R}'/E 100 '${VNUM_NEW}' 0 '${WEAR}'/g' *are

perl -pi -e 's/P 100 '${VNUM_OLD}' /P 100 '${VNUM_NEW}' /g' *are
perl -pi -e 's/G 100 '${VNUM_OLD}' 0/G 100 '${VNUM_NEW}' 0/g' *are
perl -pi -e 's/O 100 '${VNUM_OLD}' 0/O 100 '${VNUM_NEW}' 0/g' *are
perl -pi -e 's/mob equip self '${VNUM_OLD}'$/mob equip self '${VNUM_NEW}'/g' *are
perl -pi -e 's/mob equip self '${VNUM_OLD}' hold$/mob equip self '${VNUM_NEW}' hold/g' *are
perl -pi -e 's/mob oload '${VNUM_OLD}'$/mob oload '${VNUM_NEW}'/g' *are
perl -pi -e 's/mob oload '${VNUM_OLD}' i /mob oload '${VNUM_NEW}' i /g' *are
perl -pi -e 's/mob oload '${VNUM_OLD}' r$/mob oload '${VNUM_NEW}' room/g' *are
perl -pi -e 's/mob oload '${VNUM_OLD}' room$/mob oload '${VNUM_NEW}' room/g' *are
perl -pi -e 's/if objhere '${VNUM_OLD}'$/if objhere '${VNUM_NEW}'/g' *are

#perl -pi -e 's/Corpse '${VNUM_OLD}'$/Corpse '${VNUM_NEW}'/g' *are

grep " ${VNUM_OLD} " *.are
grep " ${VNUM_OLD}$" *.are -E


# WEAR_FINGER_L  -  1
# WEAR_FINGER_R  -  2
# WEAR_NECK_1    -  3
# WEAR_NECK_2    -  4
# WEAR_BODY      -  5
# WEAR_HEAD      -  6
# WEAR_LEGS      -  7
# WEAR_FEET      -  8
# WEAR_HANDS     -  9
# WEAR_ARMS      - 10
# WEAR_SHIIELD   - 11
# WEAR_ABOUT     - 12
# WEAR_WAIST     - 13
# WEAR_WIELD     - 16
# WEAR_HOLD      - 17
# WEAR_DUALWIELD - 19


EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5FBD892A
P 3050 2275
F 0 "H1" H 3150 2324 50  0000 L CNN
F 1 "MountingHole_Pad" H 3150 2233 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 3050 2275 50  0001 C CNN
F 3 "~" H 3050 2275 50  0001 C CNN
	1    3050 2275
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5FBD8D23
P 4125 2275
F 0 "H2" H 4225 2324 50  0000 L CNN
F 1 "MountingHole_Pad" H 4225 2233 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 4125 2275 50  0001 C CNN
F 3 "~" H 4125 2275 50  0001 C CNN
	1    4125 2275
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5FBD9365
P 3050 2500
F 0 "#PWR01" H 3050 2250 50  0001 C CNN
F 1 "GND" H 3055 2327 50  0000 C CNN
F 2 "" H 3050 2500 50  0001 C CNN
F 3 "" H 3050 2500 50  0001 C CNN
	1    3050 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 2500 3050 2450
Wire Wire Line
	3050 2450 4125 2450
Wire Wire Line
	4125 2450 4125 2375
Connection ~ 3050 2450
Wire Wire Line
	3050 2450 3050 2375
$EndSCHEMATC

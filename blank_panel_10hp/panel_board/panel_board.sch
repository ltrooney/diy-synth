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
U 1 1 5F827013
P 2300 2600
F 0 "H1" H 2400 2649 50  0000 L CNN
F 1 "MountingHole_Pad" H 2400 2558 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 2300 2600 50  0001 C CNN
F 3 "~" H 2300 2600 50  0001 C CNN
	1    2300 2600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5F8271F1
P 2750 2600
F 0 "H2" H 2850 2649 50  0000 L CNN
F 1 "MountingHole_Pad" H 2850 2558 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 2750 2600 50  0001 C CNN
F 3 "~" H 2750 2600 50  0001 C CNN
	1    2750 2600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5F827ECE
P 3250 2600
F 0 "H3" H 3350 2649 50  0000 L CNN
F 1 "MountingHole_Pad" H 3350 2558 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 3250 2600 50  0001 C CNN
F 3 "~" H 3250 2600 50  0001 C CNN
	1    3250 2600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5F828251
P 3750 2600
F 0 "H4" H 3850 2649 50  0000 L CNN
F 1 "MountingHole_Pad" H 3850 2558 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 3750 2600 50  0001 C CNN
F 3 "~" H 3750 2600 50  0001 C CNN
	1    3750 2600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5F82879D
P 2300 3000
F 0 "#PWR01" H 2300 2750 50  0001 C CNN
F 1 "GND" H 2305 2827 50  0000 C CNN
F 2 "" H 2300 3000 50  0001 C CNN
F 3 "" H 2300 3000 50  0001 C CNN
	1    2300 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 3000 2300 2850
Wire Wire Line
	2300 2850 2750 2850
Wire Wire Line
	2750 2850 2750 2700
Connection ~ 2300 2850
Wire Wire Line
	2300 2850 2300 2700
Wire Wire Line
	2750 2850 3250 2850
Wire Wire Line
	3250 2850 3250 2700
Connection ~ 2750 2850
Wire Wire Line
	3250 2850 3750 2850
Wire Wire Line
	3750 2850 3750 2700
Connection ~ 3250 2850
$EndSCHEMATC

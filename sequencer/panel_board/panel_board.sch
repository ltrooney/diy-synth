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
U 1 1 615B5D8F
P 3450 2675
F 0 "H1" H 3550 2724 50  0000 L CNN
F 1 "MountingHole_Pad" H 3550 2633 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 3450 2675 50  0001 C CNN
F 3 "~" H 3450 2675 50  0001 C CNN
	1    3450 2675
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 615B5FF3
P 4325 2675
F 0 "H2" H 4425 2724 50  0000 L CNN
F 1 "MountingHole_Pad" H 4425 2633 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 4325 2675 50  0001 C CNN
F 3 "~" H 4325 2675 50  0001 C CNN
	1    4325 2675
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 615B64D1
P 3450 2875
F 0 "#PWR0101" H 3450 2625 50  0001 C CNN
F 1 "GND" H 3455 2702 50  0000 C CNN
F 2 "" H 3450 2875 50  0001 C CNN
F 3 "" H 3450 2875 50  0001 C CNN
	1    3450 2875
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 2875 3450 2825
Wire Wire Line
	3450 2825 4325 2825
Wire Wire Line
	4325 2825 4325 2775
Connection ~ 3450 2825
Wire Wire Line
	3450 2825 3450 2775
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 615B7384
P 3450 3275
F 0 "H3" H 3550 3324 50  0000 L CNN
F 1 "MountingHole_Pad" H 3550 3233 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 3450 3275 50  0001 C CNN
F 3 "~" H 3450 3275 50  0001 C CNN
	1    3450 3275
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 615B738E
P 4325 3275
F 0 "H4" H 4425 3324 50  0000 L CNN
F 1 "MountingHole_Pad" H 4425 3233 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_DIN965_Pad" H 4325 3275 50  0001 C CNN
F 3 "~" H 4325 3275 50  0001 C CNN
	1    4325 3275
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 615B7398
P 3450 3475
F 0 "#PWR01" H 3450 3225 50  0001 C CNN
F 1 "GND" H 3455 3302 50  0000 C CNN
F 2 "" H 3450 3475 50  0001 C CNN
F 3 "" H 3450 3475 50  0001 C CNN
	1    3450 3475
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3475 3450 3425
Wire Wire Line
	3450 3425 4325 3425
Wire Wire Line
	4325 3425 4325 3375
Connection ~ 3450 3425
Wire Wire Line
	3450 3425 3450 3375
$EndSCHEMATC

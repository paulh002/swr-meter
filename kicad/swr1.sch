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
L tube:AD8307 U1
U 1 1 5E43CC19
P 5950 1450
F 0 "U1" V 6071 822 50  0000 R CNN
F 1 "AD8307" V 5980 822 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 6050 750 50  0001 C CNN
F 3 "" H 5950 1450 50  0001 C CNN
	1    5950 1450
	0    -1   -1   0   
$EndComp
$Comp
L tube:AD8307 U2
U 1 1 5E43D436
P 5950 3500
F 0 "U2" V 6071 2872 50  0000 R CNN
F 1 "AD8307" V 5980 2872 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 6050 2800 50  0001 C CNN
F 3 "" H 5950 3500 50  0001 C CNN
	1    5950 3500
	0    -1   -1   0   
$EndComp
$Comp
L Device:Transformer_1P_1S T1
U 1 1 5E43E809
P 1550 2050
F 0 "T1" V 1504 2294 50  0000 L CNN
F 1 "30:1" V 1595 2294 50  0000 L CNN
F 2 "" H 1550 2050 50  0001 C CNN
F 3 "~" H 1550 2050 50  0001 C CNN
	1    1550 2050
	0    1    1    0   
$EndComp
Wire Wire Line
	1750 2450 1750 3000
Wire Wire Line
	1750 3000 2100 3000
$Comp
L Device:Transformer_1P_1S T2
U 1 1 5E43E9E6
P 2500 2800
F 0 "T2" H 2500 3181 50  0000 C CNN
F 1 "30:1" H 2500 3090 50  0000 C CNN
F 2 "" H 2500 2800 50  0001 C CNN
F 3 "~" H 2500 2800 50  0001 C CNN
	1    2500 2800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1750 1650 2100 1650
Wire Wire Line
	2100 1650 2100 2600
$Comp
L power:GND #PWR02
U 1 1 5E440806
P 1750 3000
F 0 "#PWR02" H 1750 2750 50  0001 C CNN
F 1 "GND" H 1755 2827 50  0000 C CNN
F 2 "" H 1750 3000 50  0001 C CNN
F 3 "" H 1750 3000 50  0001 C CNN
	1    1750 3000
	1    0    0    -1  
$EndComp
Connection ~ 1750 3000
Wire Wire Line
	1350 2450 1350 3400
Wire Wire Line
	1350 3400 2900 3400
Wire Wire Line
	2900 3400 2900 3000
$Comp
L Device:R R4
U 1 1 5E4413FB
P 3650 3150
F 0 "R4" H 3720 3196 50  0000 L CNN
F 1 "150" H 3720 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3580 3150 50  0001 C CNN
F 3 "~" H 3650 3150 50  0001 C CNN
	1    3650 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5E441F14
P 3950 3150
F 0 "R5" H 4020 3196 50  0000 L CNN
F 1 "150" H 4020 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3880 3150 50  0001 C CNN
F 3 "~" H 3950 3150 50  0001 C CNN
	1    3950 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5E4420D6
P 4250 3150
F 0 "R6" H 4320 3196 50  0000 L CNN
F 1 "150" H 4320 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4180 3150 50  0001 C CNN
F 3 "~" H 4250 3150 50  0001 C CNN
	1    4250 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5E442398
P 3400 3150
F 0 "R3" H 3470 3196 50  0000 L CNN
F 1 "150" H 3470 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3330 3150 50  0001 C CNN
F 3 "~" H 3400 3150 50  0001 C CNN
	1    3400 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3300 3650 3300
Connection ~ 3650 3300
Wire Wire Line
	3650 3300 3800 3300
Connection ~ 3950 3300
Wire Wire Line
	3950 3300 4250 3300
Wire Wire Line
	3400 3000 3500 3000
Wire Wire Line
	3950 3000 4100 3000
$Comp
L power:GND #PWR05
U 1 1 5E444431
P 3800 3300
F 0 "#PWR05" H 3800 3050 50  0001 C CNN
F 1 "GND" H 3805 3127 50  0000 C CNN
F 2 "" H 3800 3300 50  0001 C CNN
F 3 "" H 3800 3300 50  0001 C CNN
	1    3800 3300
	1    0    0    -1  
$EndComp
Connection ~ 3800 3300
Wire Wire Line
	3800 3300 3950 3300
Wire Wire Line
	2900 2600 3500 2600
Wire Wire Line
	3500 2600 3500 3000
Connection ~ 3500 3000
Wire Wire Line
	3500 3000 3650 3000
Wire Wire Line
	2900 3000 2900 2850
Wire Wire Line
	2900 2850 4100 2850
Wire Wire Line
	4100 2850 4100 3000
Connection ~ 2900 3000
Connection ~ 4100 3000
Wire Wire Line
	4100 3000 4250 3000
$Comp
L Device:R R7
U 1 1 5E444FC0
P 4600 2600
F 0 "R7" V 4393 2600 50  0000 C CNN
F 1 "127" V 4484 2600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4530 2600 50  0001 C CNN
F 3 "~" H 4600 2600 50  0001 C CNN
	1    4600 2600
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5E445589
P 4600 2850
F 0 "R8" V 4393 2850 50  0000 C CNN
F 1 "127" V 4484 2850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4530 2850 50  0001 C CNN
F 3 "~" H 4600 2850 50  0001 C CNN
	1    4600 2850
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 2600 4450 2600
Connection ~ 3500 2600
Wire Wire Line
	4100 2850 4450 2850
Connection ~ 4100 2850
$Comp
L Device:R R9
U 1 1 5E44619E
P 5000 3150
F 0 "R9" H 5070 3196 50  0000 L CNN
F 1 "24" H 5070 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4930 3150 50  0001 C CNN
F 3 "~" H 5000 3150 50  0001 C CNN
	1    5000 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 5E446623
P 5200 3150
F 0 "R10" H 5270 3196 50  0000 L CNN
F 1 "24" H 5270 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 5130 3150 50  0001 C CNN
F 3 "~" H 5200 3150 50  0001 C CNN
	1    5200 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3300 5000 3300
Connection ~ 4250 3300
Connection ~ 5000 3300
Wire Wire Line
	5000 3300 5200 3300
Wire Wire Line
	5000 3000 5000 2600
Wire Wire Line
	5000 2600 4750 2600
Wire Wire Line
	4750 2850 5200 2850
Wire Wire Line
	5200 2850 5200 3000
$Comp
L Device:C C2
U 1 1 5E448188
P 5750 950
F 0 "C2" V 5498 950 50  0000 C CNN
F 1 "100n" V 5589 950 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 5788 800 50  0001 C CNN
F 3 "~" H 5750 950 50  0001 C CNN
	1    5750 950 
	0    1    1    0   
$EndComp
$Comp
L Device:C C1
U 1 1 5E44BBAC
P 5550 2850
F 0 "C1" V 5298 2850 50  0000 C CNN
F 1 "100n" V 5389 2850 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 5588 2700 50  0001 C CNN
F 3 "~" H 5550 2850 50  0001 C CNN
	1    5550 2850
	0    1    1    0   
$EndComp
Connection ~ 5000 2600
Wire Wire Line
	5200 2850 5400 2850
Connection ~ 5200 2850
Wire Wire Line
	5700 2850 6200 2850
Wire Wire Line
	6200 2850 6200 3000
$Comp
L power:GND #PWR07
U 1 1 5E44DE08
P 6400 4150
F 0 "#PWR07" H 6400 3900 50  0001 C CNN
F 1 "GND" H 6405 3977 50  0000 C CNN
F 2 "" H 6400 4150 50  0001 C CNN
F 3 "" H 6400 4150 50  0001 C CNN
	1    6400 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5E44F406
P 6400 2100
F 0 "#PWR06" H 6400 1850 50  0001 C CNN
F 1 "GND" H 6405 1927 50  0000 C CNN
F 2 "" H 6400 2100 50  0001 C CNN
F 3 "" H 6400 2100 50  0001 C CNN
	1    6400 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5E44F932
P 6200 1950
F 0 "C3" H 6085 1904 50  0000 R CNN
F 1 "100n" H 6085 1995 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6238 1800 50  0001 C CNN
F 3 "~" H 6200 1950 50  0001 C CNN
	1    6200 1950
	-1   0    0    1   
$EndComp
$Comp
L Device:C C5
U 1 1 5E450148
P 6500 1950
F 0 "C5" H 6385 1904 50  0000 R CNN
F 1 "100n" H 6385 1995 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6538 1800 50  0001 C CNN
F 3 "~" H 6500 1950 50  0001 C CNN
	1    6500 1950
	-1   0    0    1   
$EndComp
$Comp
L Device:C C6
U 1 1 5E450435
P 6500 4000
F 0 "C6" H 6385 3954 50  0000 R CNN
F 1 "10n" H 6385 4045 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6538 3850 50  0001 C CNN
F 3 "~" H 6500 4000 50  0001 C CNN
	1    6500 4000
	-1   0    0    1   
$EndComp
$Comp
L Device:C C4
U 1 1 5E4508D8
P 6200 4000
F 0 "C4" H 6085 3954 50  0000 R CNN
F 1 "100n" H 6085 4045 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6238 3850 50  0001 C CNN
F 3 "~" H 6200 4000 50  0001 C CNN
	1    6200 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	6200 4150 6300 4150
Wire Wire Line
	6300 3850 6300 4150
Connection ~ 6300 4150
Wire Wire Line
	6300 4150 6400 4150
Connection ~ 6400 4150
Wire Wire Line
	6400 4150 6500 4150
Wire Wire Line
	6200 2100 6300 2100
Connection ~ 6400 2100
Wire Wire Line
	6400 2100 6500 2100
Wire Wire Line
	6300 1800 6300 2100
Connection ~ 6300 2100
Wire Wire Line
	6300 2100 6400 2100
Wire Wire Line
	6300 3000 6350 3000
Text GLabel 6700 800  1    50   Input ~ 0
V33
Text GLabel 6650 2800 1    50   Input ~ 0
V33
$Comp
L Device:C C8
U 1 1 5E455A56
P 7000 3150
F 0 "C8" H 6885 3104 50  0000 R CNN
F 1 "100n" H 6885 3195 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 7038 3000 50  0001 C CNN
F 3 "~" H 7000 3150 50  0001 C CNN
	1    7000 3150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5E4560D2
P 7000 3300
F 0 "#PWR09" H 7000 3050 50  0001 C CNN
F 1 "GND" H 7005 3127 50  0000 C CNN
F 2 "" H 7000 3300 50  0001 C CNN
F 3 "" H 7000 3300 50  0001 C CNN
	1    7000 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 5E4563FC
P 7000 1100
F 0 "C7" H 6885 1054 50  0000 R CNN
F 1 "100n" H 6885 1145 50  0000 R CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 7038 950 50  0001 C CNN
F 3 "~" H 7000 1100 50  0001 C CNN
	1    7000 1100
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5E456406
P 7000 1250
F 0 "#PWR08" H 7000 1000 50  0001 C CNN
F 1 "GND" H 7005 1077 50  0000 C CNN
F 2 "" H 7000 1250 50  0001 C CNN
F 3 "" H 7000 1250 50  0001 C CNN
	1    7000 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 950  6350 950 
Wire Wire Line
	6350 950  6350 800 
Wire Wire Line
	6350 800  7000 800 
Wire Wire Line
	7000 800  7000 950 
Connection ~ 6350 950 
Wire Wire Line
	6350 950  6400 950 
Wire Wire Line
	6350 3000 6350 2800
Wire Wire Line
	6350 2800 7000 2800
Wire Wire Line
	7000 2800 7000 3000
Connection ~ 6350 3000
Wire Wire Line
	6350 3000 6400 3000
Wire Wire Line
	5900 950  6200 950 
Wire Wire Line
	5600 950  5000 950 
Wire Wire Line
	5000 950  5000 2600
$Comp
L Connector:Conn_01x06_Female J4
U 1 1 5E45EFA0
P 7900 2350
F 0 "J4" H 7928 2326 50  0000 L CNN
F 1 "Conn_01x06_Female" H 7928 2235 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 7900 2350 50  0001 C CNN
F 3 "~" H 7900 2350 50  0001 C CNN
	1    7900 2350
	1    0    0    -1  
$EndComp
Text GLabel 8000 1500 2    50   Input ~ 0
V33
$Comp
L power:GND #PWR010
U 1 1 5E45FF87
P 7700 2650
F 0 "#PWR010" H 7700 2400 50  0001 C CNN
F 1 "GND" V 7705 2522 50  0000 R CNN
F 2 "" H 7700 2650 50  0001 C CNN
F 3 "" H 7700 2650 50  0001 C CNN
	1    7700 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	6500 1800 7250 1800
Wire Wire Line
	7250 1800 7250 2250
Wire Wire Line
	7250 2250 7700 2250
Connection ~ 6500 1800
Wire Wire Line
	6500 3850 7400 3850
Wire Wire Line
	7400 3850 7400 2350
Wire Wire Line
	7400 2350 7700 2350
Connection ~ 6500 3850
$Comp
L Connector:Conn_Coaxial J1
U 1 1 5E462737
P 850 1650
F 0 "J1" H 778 1888 50  0000 C CNN
F 1 "TX" H 778 1797 50  0000 C CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 850 1650 50  0001 C CNN
F 3 " ~" H 850 1650 50  0001 C CNN
	1    850  1650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1050 1650 1350 1650
$Comp
L Connector:Conn_Coaxial J3
U 1 1 5E465426
P 2500 1650
F 0 "J3" H 2600 1625 50  0000 L CNN
F 1 "Ant" H 2600 1534 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 2500 1650 50  0001 C CNN
F 3 " ~" H 2500 1650 50  0001 C CNN
	1    2500 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5E465D29
P 2500 1850
F 0 "#PWR04" H 2500 1600 50  0001 C CNN
F 1 "GND" H 2505 1677 50  0000 C CNN
F 2 "" H 2500 1850 50  0001 C CNN
F 3 "" H 2500 1850 50  0001 C CNN
	1    2500 1850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5E466047
P 850 1850
F 0 "#PWR01" H 850 1600 50  0001 C CNN
F 1 "GND" H 855 1677 50  0000 C CNN
F 2 "" H 850 1850 50  0001 C CNN
F 3 "" H 850 1850 50  0001 C CNN
	1    850  1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1650 2300 1650
Connection ~ 2100 1650
$Comp
L Device:R R1
U 1 1 5E467538
P 2100 1350
F 0 "R1" H 2170 1396 50  0000 L CNN
F 1 "22k" H 2170 1305 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 2030 1350 50  0001 C CNN
F 3 "~" H 2100 1350 50  0001 C CNN
	1    2100 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1650 2100 1500
$Comp
L Connector:Conn_Coaxial J2
U 1 1 5E46A185
P 2500 900
F 0 "J2" H 2600 875 50  0000 L CNN
F 1 "Probe" H 2600 784 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132134_Vertical" H 2500 900 50  0001 C CNN
F 3 " ~" H 2500 900 50  0001 C CNN
	1    2500 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1200 2100 900 
Wire Wire Line
	2100 900  2300 900 
$Comp
L power:GND #PWR03
U 1 1 5E46B9EF
P 2400 1200
F 0 "#PWR03" H 2400 950 50  0001 C CNN
F 1 "GND" H 2405 1027 50  0000 C CNN
F 2 "" H 2400 1200 50  0001 C CNN
F 3 "" H 2400 1200 50  0001 C CNN
	1    2400 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5E46C09A
P 2300 1050
F 0 "R2" H 2370 1096 50  0000 L CNN
F 1 "2.2k" H 2370 1005 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2230 1050 50  0001 C CNN
F 3 "~" H 2300 1050 50  0001 C CNN
	1    2300 1050
	1    0    0    -1  
$EndComp
Connection ~ 2300 900 
Wire Wire Line
	2300 1200 2400 1200
Wire Wire Line
	2400 1200 2500 1200
Wire Wire Line
	2500 1200 2500 1100
Connection ~ 2400 1200
$Comp
L Regulator_Linear:AMS1117-3.3 U3
U 1 1 5F9ADA87
P 7700 1500
F 0 "U3" H 7700 1742 50  0000 C CNN
F 1 "AMS1117-3.3" H 7700 1651 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 7700 1700 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 7800 1250 50  0001 C CNN
	1    7700 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5F9B0218
P 7700 1800
F 0 "#PWR011" H 7700 1550 50  0001 C CNN
F 1 "GND" H 7705 1627 50  0000 C CNN
F 2 "" H 7700 1800 50  0001 C CNN
F 3 "" H 7700 1800 50  0001 C CNN
	1    7700 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 1500 7400 2150
Wire Wire Line
	7400 2150 7700 2150
$Comp
L Device:C C9
U 1 1 5F9B3889
P 2900 2450
F 0 "C9" V 2648 2450 50  0000 C CNN
F 1 "100n" V 2739 2450 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D9.0mm_W5.0mm_P5.00mm" H 2938 2300 50  0001 C CNN
F 3 "~" H 2900 2450 50  0001 C CNN
	1    2900 2450
	-1   0    0    1   
$EndComp
Connection ~ 2900 2600
$Comp
L power:GND #PWR012
U 1 1 5F9B41B3
P 2900 2300
F 0 "#PWR012" H 2900 2050 50  0001 C CNN
F 1 "GND" H 2905 2127 50  0000 C CNN
F 2 "" H 2900 2300 50  0001 C CNN
F 3 "" H 2900 2300 50  0001 C CNN
	1    2900 2300
	-1   0    0    1   
$EndComp
$EndSCHEMATC

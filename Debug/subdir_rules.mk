################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
clock.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/clock.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

eth0.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/eth0.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

gpio.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/gpio.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

spi0.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/spi0.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uart0.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/uart0.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

wait.obj: /Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries/wait.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/IOT_Project_1" --include_path="/Users/nathanfusselman/Documents/School/Spring_2021/CSE_5352/Labs_Projects/Libraries" --include_path="Libraries" --include_path="/Applications/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



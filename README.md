## Configuration and Installation
Instruction to configure environmnet and install tool requiled:  
1. Open the terminal, ctrl+alt+T or just open it from left menu
2. Change directory to **aaltaf-muc/** in the terminal
3. To compile the program, run the following command:  
		`make`
4. Test AALTAF, run following commands in terminal:  
		```./aaltaf 'a & !a'```  
    
You will see some output starting with "MUC", if you install AALTAF successfully.
Note that the entered formula should be unsatisfiable, otherwise there will be errors.

## Usage for AALTAF
After you compile AALTAF successfully, the executable file is in the directory, i.e., /aaltaf-muc/aaltaf.  
Therefore, if you want to use aaltaf, you can specify the executable file by file path in command line, or you can update environment variable:  
		`export PATH=$PATH:/home/aaltaf-muc/aaltaf/`

To run the aaltaf:  
1. Open the terminal, ctrl+alt+T or just open it from left menu
2. Change directory to aaltaf-muc/aaltaf in the terminal
3. Execute:  
	`./aaltaf -h`

    informaton about usage:  
    ```
    usage: ./aalta_f [-e|-v|-blsc|-t|-h|-naive|-naive-uc|-binary|-binary-uc|-global] ["formula"]  
	 -e	:	 print example when result is SAT  
	 -v	:	 print verbose details  
	 -blsc	:	 use the BLSC checking method; Default is CDLSC  
	 -t	:	 print weak until formula  
	 -h	:	 print help information  
	 -c	:	 open the Correctness Check  
	 -naive	:	 use the NAIVE approach to obtain MUC  
	 -naive-uc:	 use the NAIVE+UC approach to obtain MUC  
	 -binary	:	 use the BINARY approach to obtain MUC  
	 -binary-uc:	 use the BINARY+UC approach to obtain MUC  
	 -global	:	 use the GLOBAL approach to obtain MUC  
   ```
Note that if you only enter the ./aaltaf command, the BINARY method will be used by default

4. Run some examples:  
   - excute example:  
 	`./aaltaf  'a & !a'`  
    You can also just enter ./aaltaf command, and then enter the formula after the **'please input the formula:'** appears.  
   - For longer formulas, we recommend to directly enter the file path to execute, otherwise there may be a syntax error:  
    	`./aaltaf <'../benchmarks/general/spec_cl_18.alaska' `  
   - if you want to use BINARY+UC approach, try to execute:  
      	`./aaltaf -binary-uc <'../benchmarks/general/spec_cl_18.alaska' `

## Correctness Check
We have checked the correctness of the MUC obtained by each formula in the benchmarks.  
- If you want to check the correctness, try to execute:  
	`./aaltaf -c`  
- You can use this command in combination with method commands：  
	`./aaltaf -c -global 'a & !a'`  

The terminal will output "MUC is right" if the obtained MUC is correct.



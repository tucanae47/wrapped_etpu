/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <defs.h>
#include <stub.c>
// #include <stdio.h>

#define W_ADDR(addr)          		(*(volatile uint32_t*)(0x30000000 + (addr)))
#define S_ADDR(addr)          		(*(volatile uint32_t*)(0x30000100 + (addr)))
#define R_ADDR(addr)          		(*(volatile uint32_t*)(0x30000200 + (addr)))

#define PROJECT_ID 1


void set_ws(const uint8_t addr,uint32_t data){
        W_ADDR(addr * 4) = data;
}
uint32_t get_ws(const uint8_t addr){
        return W_ADDR(addr * 4);
}
void set_stream(const uint8_t addr,uint32_t data){
        S_ADDR(addr * 4) = data;
}
uint32_t get_stream(const uint8_t addr){
        return S_ADDR(addr * 4);
}
void set_readout(const uint8_t addr,uint32_t data){
        R_ADDR(addr * 4) = data;
        // R_ADDR(addr ) = data;
}
uint32_t get_readout(const uint8_t addr){
        return R_ADDR(addr * 4);
        // return R_ADDR(addr );
}


void main()
{
	/* 
	IO Control Registers
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |

	Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |
	
	 
	Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |

	*/

    // 2 inputs
	reg_mprj_io_8 =   GPIO_MODE_USER_STD_INPUT_NOPULL;
	reg_mprj_io_9 =   GPIO_MODE_USER_STD_INPUT_NOPULL;

    // 2 outputs
	// reg_mprj_io_14 =  GPIO_MODE_USER_STD_OUTPUT;
	// reg_mprj_io_15 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_21 =   GPIO_MODE_USER_STD_OUTPUT;

    /* Apply configuration */
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);

    // activate the project by setting the 0th bit of 1st bank of LA
    // reg_la0_iena = 0; // input enable off
    // reg_la0_oenb = 1; // enable logic analyser output (ignore the name, 1 is on, 0 off)
    reg_la0_iena = 0xFFFFFFFF; // input enable off
    reg_la0_oenb = 0xFFFFFFFF; // output enable on

    // enable wishbone
    reg_wb_enable  = 1;
    // reg_la0_data |= (1 << PROJECT_ID); // enable the project

    // reg_la0_iena = 0; // input enable off
    // reg_la0_oenb = 0; // output enable bar low (enabled)

    // reg_wb_enable  = 1;


    // Input
    // uint8_t W[3][3] = {{1, 4, 5},
    //     {5, 8, 9},
    //     {6, 7, 11}};

    uint8_t Wt[3][3]  = {{1, 5, 6},
                        {4, 8, 7},
                        {5, 9, 11}};

    uint8_t I[3][3]  = {{1, 5, 12},
                        {5, 9, 0},
                        {6, 11, 19}};
    // no need for anything else as this design is free running.
    // for (uint8_t i =0; i< 9; i++)


    reg_la0_data |= (1 << PROJECT_ID); // enable the project
    // for (uint8_t i =0; i< 9; i++){
    //     config_generator(i, 10+i, 4+i, ((1+i)%2));
    //     uint32_t d1= read_regs(i);
    // }
    uint8_t indexes[9][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}};
    for (uint8_t i =0; i< 9; i++){
        int x,y;
        x = indexes[i][0];
        y = indexes[i][1];
        set_ws(i, Wt[x][y]);
        // uint32_t d1= get_data(i);
    }

    uint32_t w_data4 = I[0][0];
    set_stream(0,w_data4);

    uint32_t w_data5 = I[1][0] << 8 | I[0][1];
    set_stream(1,w_data5);

    uint32_t w_data6 = I[2][0] << 16 | I[1][1] << 8 | I[0][2];
    set_stream(2,w_data6);

    uint32_t w_data7 = I[2][1] << 16 | I[1][2] << 8;
    set_stream(3,w_data7);

    uint32_t w_data8 = 1<<24 | I[2][2] << 16;
    set_stream(4,w_data8);

    // for (uint8_t i =0; i< 9; i++){
    //     set_stream(i, (127 | 127<< 8 | 127<< 16));
    //     // uint32_t d1= get_data(i);
    // }
    for (uint8_t i =5; i< 24; i++){
        uint32_t d1= get_readout(i);
    }
    set_stream(4,0);

    //Ws
    // uint32_t w_data1 = Wt[1][0] << 24 | Wt[0][2] << 16 | Wt[0][1] << 8 | Wt[0][0];
    // test_wb_set(w_data1);
    // uint32_t w_data2 = Wt[2][1] << 24 | Wt[2][0] << 16 | Wt[1][2] << 8 | Wt[1][1];
    // test_wb_set(w_data2);
    // uint32_t w_data3 = Wt[2][2];
    // test_wb_set(w_data3);
    // test_wb_set(0);

    // //Coefs
    // uint32_t w_data4 = I[0][0];
    // test_wb_set(w_data4);
    // uint32_t w_data5 = I[1][0] << 8 | I[0][1];
    // test_wb_set(w_data5);
    // uint32_t w_data6 = I[2][0] << 16 | I[1][1] << 8 | I[0][2];
    // test_wb_set(w_data6);
    // uint32_t w_data7 = I[2][1] << 16 | I[1][2] << 8;
    // test_wb_set(w_data7);
    // uint32_t w_data8 = I[2][2] << 16;
    // test_wb_set(w_data8);
}


-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xzvara01
--
library ieee;
use ieee.std_logic_1164.all;
use std.textio.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK      : in std_logic;
   RST      : in std_logic;
   DIN 	    : in std_logic;
   CLK_CNT  : in std_logic_vector(4 downto 0);   -- vector for counting clock signals
   BIT_CNT  : in std_logic_vector(3 downto 0);   -- vector for counting received bits
   CNT_EN   : out std_logic;                     -- signal to enable counting clock signals
   READ_EN  : out std_logic;					 -- start receiving actual bits
   DOUT_VLD : out std_logic						 -- valid output
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
-- all the possible states for the FSM
type STATES is (WAIT_FOR_START, START_BIT, RECEIVE_DATA, STOP_BIT, DOUT_VALID);
signal cur_state : STATES := WAIT_FOR_START;
begin

	CNT_EN <= '1' when cur_state = START_BIT or cur_state = RECEIVE_DATA 
					or cur_state = STOP_BIT else '0';
					
	READ_EN <= '1' when cur_state = RECEIVE_DATA else '0';
	
	DOUT_VLD <= '1' when cur_state = DOUT_VALID else '0';
	
	set_state : process (CLK)
	begin
		if CLK'event AND CLK = '0' then
			if RST = '1' then
				cur_state <= WAIT_FOR_START;
			else	
			
				case cur_state is
					when WAIT_FOR_START =>
						if DIN = '0' then
							cur_state <= START_BIT;
						end if;
						
					when START_BIT =>
						if CLK_CNT = "11000" then
							cur_state <= RECEIVE_DATA;
						end if;
						
					when RECEIVE_DATA => 
						if BIT_CNT = "1000" then
							cur_state <= STOP_BIT;
						end if;
						
					when STOP_BIT =>
						if CLK_CNT = "11000" then
							cur_state <= DOUT_VALID;
						end if;
						
					when DOUT_VALID =>
						cur_state <= WAIT_FOR_START;
					
					when others => null;
				end case;
				
			end if; -- RST signal
		end if; -- rising edge of CLK
	end process set_state;
	
end behavioral;

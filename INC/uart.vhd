-- uart.vhd: UART controller - receiving part
-- Author(s): xzvara01
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-------------------------------------------------
entity UART_RX is
port(	
    CLK		 : in std_logic;
	RST		 : in std_logic;
	DIN		 : in std_logic;
	DOUT     : out std_logic_vector(7 downto 0);
	DOUT_VLD : out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal clk_cnt  : std_logic_vector(4 downto 0) := "00000";
signal bit_cnt  : std_logic_vector(3 downto 0) := "0000";
signal cnt_en   : std_logic;
signal read_en  : std_logic;
signal do_vld   : std_logic;
begin

	FSM: entity work.UART_FSM
	port map (CLK => CLK, 
			  RST => RST,
			  DIN => DIN,
			  CLK_CNT => clk_cnt,
			  BIT_CNT => bit_cnt,
			  CNT_EN => cnt_en,
			  READ_EN => read_en,
			  DOUT_VLD => do_vld
			 );

			 
	clk_cycles : process (CLK)
	begin
		if rising_edge(CLK) then
			if cnt_en = '1' then
				clk_cnt <= clk_cnt + 1;
				
				if read_en = '1' and (clk_cnt(3 downto 0) = "1111" or clk_cnt(4) = '1') then
					bit_cnt <= bit_cnt + 1;
					clk_cnt <= "00000";
				end if;
			else
				clk_cnt <= "00000";
				if do_vld = '0' then
					bit_cnt <= "0000";
				end if;
			end if;
		end if; -- CLK rising edge
	end process clk_cycles;
	
	write_out : process (CLK)
	begin
		if rising_edge(CLK) then
			if read_en = '1' and (clk_cnt(3 downto 0) = "1111" or clk_cnt(4) = '1') then
					case bit_cnt(2 downto 0) is
						when "000" => DOUT(0) <= DIN;
						when "001" => DOUT(1) <= DIN;
						when "010" => DOUT(2) <= DIN;
						when "011" => DOUT(3) <= DIN;
						when "100" => DOUT(4) <= DIN;
						when "101" => DOUT(5) <= DIN;
						when "110" => DOUT(6) <= DIN;
						when "111" => DOUT(7) <= DIN;
						
						when others => null;
					
					end case;
			end if;
		end if; -- CLK rising edge
	end process write_out;

	valid : process (do_vld)
	begin
		DOUT_VLD <= do_vld;
	end process;
	
end behavioral;




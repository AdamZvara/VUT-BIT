-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xzvara01
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- PC
	signal pc_reg : std_logic_vector(11 downto 0);	-- pristup do ROM code_addr
	signal pc_inc : std_logic;								
	signal pc_dec : std_logic;
 -- PC
 
 -- CNT
	signal cnt_reg  : std_logic_vector(7 downto 0);	-- pocitadlo vnorenych while cyklov, aktualne 8bitove = 256 hodnot
	signal cnt_inc  : std_logic;
	signal cnt_dec  : std_logic;
	signal cnt_zero : std_logic; 
 -- CNT
 
 -- PTR
	signal ptr_reg : std_logic_vector(9 downto 0);	-- pristup do RAM data_addr
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
 -- PTR
 
 -- RDATA zero
	signal rdata_zero : std_logic;
 -- RDATA zero
 
 -- FSM STATES
	type fsm_state is (
		sidle, sfetch, sdecode, 													-- cakanie, fetch, dekodovanie
		sptr_inc, sptr_dec, 															-- > <
		sval_inc, sval_inc_load, sval_inc_muxset,								-- +
		sval_dec, sval_dec_muxset, sval_dec_load,								-- -
		swhile_start, swhile_skip, swhile_start_wait,						-- [
		swhile_end, swhile_backtrack, sbacktrack_wait, swhile_wait,		-- ]
		swrite, swrite_finish, sread, sread_finish,							-- . ,
		sbreak, sbreak_loop,															-- ~
		snull,
		scomment
	);
	
	signal pstate : fsm_state := sidle;
	signal nstate : fsm_state;
 -- FSM STATES
 
 -- MUX
	signal mux_sel : std_logic_vector (1 downto 0) := (others => '0');
	signal mux_out : std_logic_vector (7 downto 0) := (others => '0');
 -- MUX
 
 -- DECODER
	type inst_type is (
		iptr_inc, iptr_dec, 
		ival_inc, ival_dec,
		iwhile_start, iwhile_end,
		iwrite, iread,
		ibreak, 
		inull,
		icomment
	);
	
	signal ireg_dec : inst_type;
 -- DECODER

begin

	rdata_zero <= '1' when (DATA_RDATA = "0000000") else '0';

 -- PC 
	pc: process(CLK, RESET)
	begin
		if (RESET = '1') then
			pc_reg <= (others => '0');
		elsif rising_edge(CLK) then
			if (pc_inc = '1') then
				pc_reg <= pc_reg + 1;
			elsif (pc_dec = '1') then	-- not sure if this is okay
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	CODE_ADDR <= pc_reg;
 -- PC
 
 -- PTR 
	ptr: process(CLK, RESET)
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif rising_edge(CLK) then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then	-- not sure if this is okay
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	DATA_ADDR <= ptr_reg;
 -- PTR
 
 -- CNT
	cnt: process(CLK, RESET)
	begin
		if (RESET = '1') then
			cnt_reg <= (others => '0');
		elsif rising_edge(CLK) then
			if (cnt_inc = '1') then
				cnt_reg <= cnt_reg + 1;
			elsif (cnt_dec = '1') then	-- not sure if this is okay
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;
	cnt_zero <= '1' when (cnt_reg = "00000000") else '0';
 -- CNT
 
 -- DECODER
	decoder: process(CODE_DATA)
	begin
		case (CODE_DATA) is	
			when X"3E" => ireg_dec <= iptr_inc;			-- PTR_INC
			when X"3C" => ireg_dec <= iptr_dec;			-- PTR_DEC
			when X"2B" => ireg_dec <= ival_inc; 		-- VALUE_INC
			when X"2D" => ireg_dec <= ival_dec; 		-- VALUE_DEC
			when X"2E" => ireg_dec <= iwrite; 			-- WRITE
			when X"2C" => ireg_dec <= iread; 			-- READ
			when X"5B" => ireg_dec <= iwhile_start;	-- WHILE_START
			when X"5D" => ireg_dec <= iwhile_end;		-- WHILE_END	
			when X"7E" => ireg_dec <= ibreak;  			-- BREAK
			when X"00" => ireg_dec <= inull;				-- NULL
			when others => ireg_dec <= icomment;		-- comments
		end case;
	end process;
 -- DECODER
 
 -- MUX
	mux: process(CLK, RESET, mux_sel)
	begin
		
		if (RESET = '1') then
			mux_out <= (others => '0');
			
		elsif rising_edge(CLK) then
			case mux_sel is
				when "00" 	=> mux_out <= IN_DATA;
				when "01" 	=> mux_out <= DATA_RDATA - 1;
				when "10" 	=> mux_out <= DATA_RDATA + 1;
				when others => mux_out <= (others => '0');
			end case;
		end if;
	
	end process;
	DATA_WDATA <= mux_out;
 -- MUX


 -- FSM
	-- nastavenie aktualneho stavu FSM
	fsm_pstate: process (CLK, RESET)
	begin
		if (RESET = '1') then									-- asynchronny reset
			pstate <= sidle;
		elsif rising_edge(CLK) then 			-- synchronne reagujem na nabeznu hranu CLK
			if (EN = '1') then
				pstate <= nstate;
			end if;
		end if;
	end process;
	

	-- TODO sensitivity list
	next_state_logic: process (pstate, ireg_dec, cnt_zero, rdata_zero, OUT_BUSY, IN_VLD)
	begin
		-- INITIALIZATION
		pc_inc  <= '0';
		pc_dec  <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
	   mux_sel  <= "00";
		
		CODE_EN   <= '0';
		IN_REQ    <= '0';
		OUT_WREN  <= '0';
		DATA_WREN <= '0';
		DATA_EN   <= '0';
		
		case pstate is
			-- IDLE
			when sidle =>
				nstate <= sfetch;
				
			-- FETCH
			when sfetch =>
				CODE_EN <= '1';
				nstate <= sdecode;
				
			-- DECODE 
			when sdecode =>
				case ireg_dec is
					when inull =>
						nstate <= snull;
					when iptr_inc =>
						nstate <= sptr_inc;
					when iptr_dec =>
						nstate <= sptr_dec;
					when ival_inc =>
						nstate <= sval_inc;
					when ival_dec =>
						nstate <= sval_dec;
					when iwhile_start =>
						DATA_EN <= '1';	-- prirava citania z RAM, kontrola podmienky cyklu RAM[x] == 0
						DATA_WREN <= '0';
						nstate <= swhile_start;
					when iwhile_end =>
						DATA_EN <= '1';	-- priprava citania z RAM, kontrola podmienky cyklu RAM[x] == 0
						DATA_WREN <= '0';
						nstate <= swhile_end;
					when iwrite =>
						nstate <= swrite;
					when iread =>
						nstate <= sread;
					when ibreak =>
						nstate <= sbreak;
					when icomment =>
						nstate <= scomment;
					when others =>
						nstate <= sidle;
				end case;
			
			-- COMMENT
			when scomment =>
				pc_inc <= '1';
				nstate <= sfetch;
				
			-- NULL
			when snull =>
				nstate <= snull;
				
			-- PTR INCREMENT
			when sptr_inc =>
				ptr_inc <= '1';
				pc_inc <= '1';
				nstate <= sfetch;
				
			-- PTR DECREMENT
			when sptr_dec =>
				ptr_dec <= '1';
				pc_inc <= '1';
				nstate <= sfetch;
				
			-- VALUE INCREMENT
			when sval_inc =>
				DATA_EN <= '1';
				DATA_WREN <= '0'; -- citanie z RAM
				nstate <= sval_inc_muxset;
				
			when sval_inc_muxset =>
				mux_sel <= "10"; -- nastavenie MUX na vyber hodnoty + 1
				nstate <= sval_inc_load;
			
			when sval_inc_load =>
				DATA_EN <= '1';
				DATA_WREN <= '1';	-- zapis do RAM
				pc_inc <= '1';
				nstate <= sfetch;
				
			-- VALUE DECREMENT
			when sval_dec =>
				DATA_EN <= '1';
				DATA_WREN <= '0'; -- citanie z RAM
				nstate <= sval_dec_muxset;
				
			when sval_dec_muxset =>
				mux_sel <= "01";	-- nastavenie MUX na vyber hodnoty - 1
				nstate <= sval_dec_load;
			
			when sval_dec_load =>
				DATA_EN <= '1';
				DATA_WREN <= '1'; 	-- zapis do RAM
				pc_inc <= '1';
				nstate <= sfetch;
			
			-- WRITE VALUE
			when swrite =>
				DATA_EN <= '1';
				DATA_WREN <= '0';	-- citanie z RAM
				nstate <= swrite_finish;
				
			when swrite_finish =>
				if (OUT_BUSY = '1') then
					DATA_EN <= '1';
					DATA_WREN <= '0';	-- citanie z RAM
					nstate <= swrite_finish;	
				else
					OUT_WREN <= '1';
					OUT_DATA <= DATA_RDATA;
					pc_inc <= '1';
					nstate <= sfetch;
				end if;
				
			-- READ VALUE
			when sread =>
				IN_REQ <= '1';
				nstate <= sread_finish;
				
			when sread_finish =>
				if (IN_VLD = '0') then
					IN_REQ <= '1';
					nstate <= sread_finish;
				else
					DATA_EN <= '1';
					DATA_WREN <= '1'; -- zapis do RAM
					pc_inc <= '1';
					nstate <= sfetch;
				end if;
				
			-- WHILE LOOP START
			when swhile_start =>
				pc_inc <= '1'; 
				if (rdata_zero = '1') then -- preskcenie cyklus v procese while_skip
					CODE_EN <= '1';			-- nastavenie code_en aby som v dalsom takte mohol citat z ROM
					cnt_inc <= '1';			-- nastavenie CNT na 1
					nstate <= swhile_start_wait;  -- skipovanie tela cyklu
				else								-- vykonavanie tela cyklu
					nstate <= sfetch;
				end if;

			when swhile_start_wait =>
				CODE_EN <= '1';
				nstate <= swhile_skip;

			when swhile_skip =>
				CODE_EN <= '1';				-- priprava citania RAM v dalsom takte
				if (cnt_zero = '1') then	-- CNT je na 0 -> preskocil som vsetky cykly
					pc_dec <= '1';
					nstate <= sfetch;
				else
					pc_inc <= '1';					-- vo vsetkych pripadoch sa budem posuvat na dalsi znak
					if (ireg_dec = iwhile_end) then				-- narazil som na ]
						cnt_dec <= '1';								-- inkrementujem cnt
						nstate <= swhile_skip;
					elsif (ireg_dec = iwhile_start) then		-- narazil som na [
						cnt_inc <= '1';								-- dekrementujem cnt
						nstate <= swhile_skip;
					else
						nstate <= swhile_skip;
					end if;
				end if;
				
			-- WHILE LOOP END
			when swhile_end =>
				if (rdata_zero = '1') then	-- koniec cyklu
					pc_inc <= '1';
					nstate <= sfetch;
				else
					-- citanie spatne az na zaciatok cyklu
					CODE_EN <= '1';	-- spristupnenie RAM v dalsom takte
					pc_dec <= '1';		-- dekrementacia pc
					cnt_inc <= '1';	-- inkrementacia cnt
					nstate <= swhile_wait;
				end if;
				
			when swhile_wait =>	-- pomocny stav na nacitanie instrukcie z ROM
				CODE_EN <= '1';
				nstate <= swhile_backtrack;
				
			when swhile_backtrack =>
				if (cnt_zero = '0') then
					pc_dec <= '1';	
					CODE_EN <= '1';									-- spristupnenie RAM v dalsom takte
					if (ireg_dec = iwhile_start) then	 		-- narazil som na [
							cnt_dec <= '1';
							-- pc_dec <= '0';								-- nechcem nacitat predchadzajuci znak ale pokracovat v citani
					elsif (ireg_dec = iwhile_end) then 			-- narazil som na ] 
							cnt_inc <= '1';	
					end if;
					nstate <= swhile_backtrack;
				else
					pc_inc <= '1';
					CODE_EN <= '1';
					nstate <= sbacktrack_wait;
				end if;
				
			when sbacktrack_wait =>
				pc_inc <= '1';
				nstate <= sfetch;
			
			-- BREAK
			when sbreak =>
				cnt_inc <= '1';			-- nastavenie CNT na 1
				pc_inc <= '1';				-- inkrementacia pc
				CODE_EN <= '1';			-- spristupnenie RAM v dalsom takte
				nstate <= sbreak_loop;
			
			when sbreak_loop =>
				if (cnt_zero = '0') then
					if (ireg_dec = iwhile_start) then 		-- narazil som na [
						cnt_inc <= '1';
					elsif (ireg_dec = iwhile_end) then	 	-- narazil som na ]
						cnt_dec <= '1';
					end if;
					pc_inc <= '1';
					CODE_EN <= '1';
					nstate <= sbreak_loop;
				else										-- koniec cyklov
					pc_dec <= '1';
					nstate <= sdecode;
				end if;
			
			when others => null;
				
		end case;
		
	end process;

 -- FSM
end behavioral;
 

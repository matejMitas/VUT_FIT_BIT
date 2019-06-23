-- fsm.vhd: Finite State Machine
-- Author(s): Matej Mitas - xmitas02
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (
		TEST1, 
		TEST2,
		TEST3,
		TEST4,
		TEST5,
		TEST61, TEST62,
		TEST71, TEST72,
		TEST81, TEST82,	
		TEST91, TEST92,
		TEST101, TEST102,
		TEST112,
		WRONG,
		PRINT_OK,
		PRINT_WRONG, 
		FINISH
	);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(8) = '1') then
				next_state <= TEST2;
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
      end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST2 =>	
		next_state <= TEST2;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(9) = '1') then
				next_state <= TEST3;	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST3 =>	
		next_state <= TEST3;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(7) = '1') then
				next_state <= TEST4;
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST4 =>	
		next_state <= TEST4;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(1) = '1') then
				next_state <= TEST5;	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST5 =>	
		next_state <= TEST5;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(5) = '1') then
				next_state <= TEST61;	
		elsif (KEY(6) = '1') then
				next_state <= TEST62;	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST61 =>	
		next_state <= TEST61;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(7) = '1') then
				next_state <= TEST71;		
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;	
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST71 =>	
		next_state <= TEST71;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(8) = '1') then
				next_state <= TEST81;		
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;	
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST81 =>	
		next_state <= TEST81;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(8) = '1') then
				next_state <= TEST91;
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;	
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST91 =>	
		next_state <= TEST91;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(5) = '1') then
				next_state <= TEST101; 	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST101 =>	
		next_state <= TEST101;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST62 =>	
		next_state <= TEST62;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(4) = '1') then
				next_state <= TEST72;		
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
	
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST72 =>	
		next_state <= TEST72;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(6) = '1') then
				next_state <= TEST82;		
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;	
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST82 =>	
		next_state <= TEST82;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(5) = '1') then
				next_state <= TEST92;
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;	
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST92 =>	
		next_state <= TEST92;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(9) = '1') then
				next_state <= TEST102; 	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST102 =>	
		next_state <= TEST102;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG; 
		elsif (KEY(4) = '1') then
				next_state <= TEST112; 	
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
	
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when TEST112 =>	
		next_state <= TEST112;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
		  next_state <= WRONG;
		end if;
	
	-- - - - - - - - - - - - - - - - - - - - - - -
   when WRONG =>
      next_state <= WRONG;
      if (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
      end if;
		
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_WRONG =>
      next_state <= PRINT_WRONG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
		
	-- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
		
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
	  FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		
	-- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_WRONG =>
	  FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';	
		
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;


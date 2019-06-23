-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Matej Mitas, xmitas02
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
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

    -- ------------------------------------------------------------------------
    --                      Signaly
    -- ------------------------------------------------------------------------   

    -- signaly pro program counter
    signal mc_PC_register           : std_logic_vector(11 downto 0);    -- velikost 4kB, 12b staci
    signal mc_PC_increment          : std_logic;                        -- povoleni INC programoveho citace
    signal mc_PC_decrement          : std_logic;                        -- povoleni DEC programoveho citace

    -- signaly pro ukazatel
    signal mc_PTR_register          : std_logic_vector(9 downto 0);     -- data v RAM jsou pouze 8 bitova, nicmene zvoleny jako 10b
    signal mc_PTR_increment         : std_logic;                        -- povoleni INC ukazatele do RAM
    signal mc_PTR_decrement         : std_logic;                        -- povoleni DEC ukazatele do RAM

    -- signaly pro citac zavorek cyklu
    signal mc_CNT_register          : std_logic_vector(7 downto 0);     -- zde velikost zalezi na poctu zanoreni, podporujeme 255 urovni
    signal mc_CNT_increment         : std_logic;                        -- povoleni INC citace zanoreni
    signal mc_CNT_decrement         : std_logic;                        -- povoleni DEC citace zanoreni
    signal mc_CNT_out               : std_logic;                        -- vystup citace, pomocny signal do radice

    -- signal pro vystupni multiplexer
    signal mc_MUX_sel               : std_logic_vector(1 downto 0);     -- dvoubitova hodnota pro vyber adresy

    -- signal pro prenos dat
    signal mc_HLP_data              : std_logic;                        -- pomocny signal pro radic k monitorovani protekajicich dat

    -- instrukce
    type ins_type is (
        mc_INC_PTR,
        mc_DEC_PTR,
        mc_INC_VAL,
        mc_DEC_VAL,
        mc_LOP_STR,
        mc_LOP_END,
        mc_PUTCHR,
        mc_GETCHR,
        mc_LOP_BRK,
        mc_END,
        mc_UNKNOWN
    );

    signal mc_instruction           : ins_type;

    -- ------------------------------------------------------------------------
    --                      Stavovy automat
    -- ------------------------------------------------------------------------  

    type mc_fsm_state is (
        mc_fsm_START,
        mc_fsm_FETCH,
        mc_fsm_BACK,
        --
        mc_fsm_DECODE,
        --
        mc_fsm_INC_PTR,
        mc_fsm_DEC_PTR,
        --
        mc_fsm_INC_VAL_READ,
        mc_fsm_DEC_VAL_READ,
        mc_fsm_INC_VAL_WRT,
        mc_fsm_DEC_VAL_WRT,
        --
        mc_fsm_LOP_STR,
        mc_fsm_LOP_STR_2,
        mc_fsm_LOP_STR_COND_1,
        mc_fsm_LOP_STR_COND_2,
        mc_fsm_LOP_STR_WHILE,
        mc_fsm_LOP_STR_WHILE_2,
        --
        mc_fsm_LOP_END,
        mc_fsm_LOP_END_COND_1,
        mc_fsm_LOP_END_COND_2,
        mc_fsm_LOP_END_WHILE,
        mc_fsm_LOP_END_WHILE_2,
        --
        mc_fsm_LOP_BRK,
        mc_fsm_LOP_BRK_COND_1,
        mc_fsm_LOP_BRK_WHILE,
        --
        mc_fsm_GETCHR,
        mc_fsm_PUTCHR_READ,
        mc_fsm_PUTCHR_WRT,
        --
        mc_fsm_END,
        mc_fsm_SKIP,
        mc_fsm_STOP
    );

    -- signaly pro statovy automat
    signal mc_pstate: mc_fsm_state;
    signal mc_nstate: mc_fsm_state;

begin

    -- ------------------------------------------------------------------------
    --                       Program citac
    -- ------------------------------------------------------------------------ 
    
    mc_PC: process(CLK, RESET)
    begin
        if (RESET = '1') then
            mc_PC_register <= (others => '0');
        elsif (CLK'event) and (CLK='1') then
            if (mc_PC_increment = '1') then
                mc_PC_register <= mc_PC_register + 1;
            elsif (mc_PC_decrement = '1') then
                mc_PC_register <= mc_PC_register - 1;
            end if;
        end if;
    end process;

    -- ------------------------------------------------------------------------
    --                      Instrukcni citac
    -- ------------------------------------------------------------------------ 
   
    mc_PTR: process(CLK, RESET)
    begin
        if (RESET = '1') then
            mc_PTR_register <= (others => '0');
        elsif (CLK'event) and (CLK='1') then
            if (mc_PTR_increment = '1') then
                mc_PTR_register <= mc_PTR_register + 1;
            elsif (mc_PTR_decrement = '1') then
                mc_PTR_register <= mc_PTR_register - 1;
            end if;
        end if;
    end process;

    -- ------------------------------------------------------------------------
    --                      Citac pro cykly
    -- ------------------------------------------------------------------------ 
    
    mc_CNT: process(CLK, RESET)
    begin
        if (RESET = '1') then
            mc_CNT_register <= (others => '0');
        elsif (CLK'event) and (CLK='1') then
            if (mc_CNT_increment = '1') then
                mc_CNT_register <= mc_CNT_register + 1;
            elsif (mc_CNT_decrement = '1') then
                mc_CNT_register <= mc_CNT_register - 1;
            end if;
        end if;
    end process;

    -- ------------------------------------------------------------------------
    --                        Vystupni MUX
    -- ------------------------------------------------------------------------ 

    mc_MUX_out: process(CLK, mc_MUX_sel, DATA_RDATA, IN_DATA)
    begin
        case mc_MUX_sel is
            when "00" => DATA_WDATA <= IN_DATA;
            when "01" => DATA_WDATA <= DATA_RDATA + 1; -- logicky clen dle schematu
            when "10" => DATA_WDATA <= DATA_RDATA - 1; -- logicky clen dle schematu
            when "11" => DATA_WDATA <= (others => '0');
            when others => null;
        end case;
    end process;

    -- ------------------------------------------------------------------------
    --                        Transfer dat
    -- ------------------------------------------------------------------------ 

    -- ulozeni kodu
    CODE_ADDR <= mc_PC_register;    -- ulozim adresu do registru
    DATA_ADDR <= mc_PTR_register;   -- ulozim data do registru

    -- ukladani dat pro komunikace s radicem
    mc_CNT_out <= '1' when (mc_CNT_register = "00000000") else '0'; -- citac zavorek pro cykly
    mc_HLP_data <= '1' when (DATA_RDATA = "00000000") else '0';     -- rozhrani ctenych dat

    -- ------------------------------------------------------------------------
    --                        Dekodovani instrukci
    -- ------------------------------------------------------------------------ 

    mc_INS: process(CODE_DATA)
    begin
        case CODE_DATA is
            when X"3E" => mc_instruction <= mc_INC_PTR;     -- >
            when X"3C" => mc_instruction <= mc_DEC_PTR;     -- <
            when X"2B" => mc_instruction <= mc_INC_VAL;     -- +
            when X"2D" => mc_instruction <= mc_DEC_VAL;     -- -
            when X"5B" => mc_instruction <= mc_LOP_STR;     -- [
            when X"5D" => mc_instruction <= mc_LOP_END;     -- ]
            when X"2E" => mc_instruction <= mc_PUTCHR;      -- .
            when X"2C" => mc_instruction <= mc_GETCHR;      -- ,
            when X"7E" => mc_instruction <= mc_LOP_BRK;     -- break
            when X"00" => mc_instruction <= mc_END;         -- null
            when others => mc_instruction <= mc_UNKNOWN;    -- nerozpoznany stav
        end case;
    end process;

    -- ------------------------------------------------------------------------
    --                              FSM
    -- ------------------------------------------------------------------------

    -- soucasny stav
    fsm_pstate: process(CLK, RESET, EN)
    begin
        if (RESET = '1') then
            mc_pstate <= mc_fsm_START;
        elsif (CLK'event) and (CLK='1') then
            if (EN = '1') then
                mc_pstate <= mc_nstate;
            end if;            
        end if;
    end process;

    -- dalsi stav (rozhodujeme se, kam dal)
    fsm_nstate: process(CLK, RESET, mc_pstate, DATA_RDATA, CODE_DATA, OUT_BUSY, IN_VLD, mc_CNT_out, mc_HLP_data, mc_MUX_sel)
    begin

        -- ------------------------------------------------------------------------
        --                    Priprava signal pred spustenim FSM
        -- ------------------------------------------------------------------------

        -- reset cteni/zapisu dat
        DATA_EN <= '0';
        DATA_RDWR <= '0';
        -- neurceny stav, nevime kam defaultne budeme zapisovat, proto zvolime 'nedovolenou kombinaci'
        mc_MUX_sel <= "11";
        -- zapis na LCD
        OUT_WE <= '0';
        -- registr preruseni
        IN_REQ <= '0';
        -- vynuluje povoleni pristupu pameti programu
        CODE_EN <= '0';
        -- nedelame nic s PC
        mc_PC_increment <= '0';
        mc_PC_decrement <= '0';
        -- nedelame nic s ukazateli
        mc_PTR_increment <= '0';
        mc_PTR_decrement <= '0';
        -- nedelame nic s citacem zavorek
        mc_CNT_increment <= '0';
        mc_CNT_decrement <= '0';

        -- stavovy automat
        case mc_pstate is 

            -- ------------------------------------------------------------------------
            --                        Pocatecni stavy radice
            -- ------------------------------------------------------------------------ 

            when mc_fsm_START =>
                mc_nstate <= mc_fsm_FETCH;

            when mc_fsm_FETCH =>
                -- povolime cteni z pameti dat
                CODE_EN <= '1';
                -- presuneme se do dalsiho stavu
                mc_nstate <= mc_fsm_DECODE;

            -- ------------------------------------------------------------------------
            --                      Pomocne rozhrani pro dekoder
            -- ------------------------------------------------------------------------ 

            when mc_fsm_DECODE =>
                -- jaky je to typ instrukce
                case mc_instruction is
                    -- nejdrive si vyresime nestandartni stavy
                    when mc_UNKNOWN =>
                        mc_nstate <= mc_fsm_SKIP;
                    when mc_END     =>
                        mc_nstate <= mc_fsm_STOP;
                    -- ukazatel a hodnota
                    when mc_INC_PTR =>
                        mc_nstate <= mc_fsm_INC_PTR;
                    when mc_DEC_PTR =>
                        mc_nstate <= mc_fsm_DEC_PTR;
                    when mc_INC_VAL =>
                        mc_nstate <= mc_fsm_INC_VAL_READ;
                    when mc_DEC_VAL =>
                        mc_nstate <= mc_fsm_DEC_VAL_READ;
                    --  cykly
                    when mc_LOP_STR =>
                        mc_nstate <= mc_fsm_LOP_STR;
                    when mc_LOP_END =>
                        mc_nstate <= mc_fsm_LOP_END;
                    when mc_LOP_BRK =>
                        mc_nstate <= mc_fsm_LOP_BRK;
                    -- I/O
                    when mc_GETCHR =>
                        mc_nstate <= mc_fsm_GETCHR;
                    when mc_PUTCHR =>
                        mc_nstate <= mc_fsm_PUTCHR_READ;
                end case;

            -- ------------------------------------------------------------------------
            --                          Special states
            -- ------------------------------------------------------------------------ 

            -- neplatna instrukce, proste preskoc
            when mc_fsm_SKIP =>
                mc_PC_increment <= '1';
                -- vrat se zpatky na FETCH
                mc_nstate <= mc_fsm_FETCH;

            -- skok zpatky na fetch, je potreba posunout 
            when mc_fsm_BACK =>
                -- posun program counter
                mc_PC_increment <= '1';
                -- 
                mc_nstate <= mc_fsm_FETCH;

            -- konec programu
            when mc_fsm_STOP =>
                mc_PC_decrement <= '0';
                mc_PC_increment <= '0';
                mc_nstate   <= mc_fsm_STOP;

            -- ------------------------------------------------------------------------
            --                              PTR INC
            -- ------------------------------------------------------------------------ 

            when mc_fsm_INC_PTR =>
                -- nahoru ukazatel
                mc_PTR_increment <= '1';
                -- 
                mc_nstate <= mc_fsm_BACK;

            -- ------------------------------------------------------------------------
            --                              PTR DEC
            -- ------------------------------------------------------------------------ 

            when mc_fsm_DEC_PTR =>
                mc_PTR_decrement <= '1';
                --
                mc_nstate <= mc_fsm_BACK;

            -- ------------------------------------------------------------------------
            --                              VAL INC
            -- ------------------------------------------------------------------------ 
            -- zde ctu
            when mc_fsm_INC_VAL_READ =>
                -- cteme
                DATA_EN   <= '1';
                DATA_RDWR <= '0';
                -- zapis
                mc_nstate <= mc_fsm_INC_VAL_WRT;

            -- zde zapisuji
            when mc_fsm_INC_VAL_WRT =>
                -- zapisujeme
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                -- pricti data
                mc_MUX_sel <= "01";
                -- zpatky na zacatek
                mc_nstate <= mc_fsm_BACK;

            -- ------------------------------------------------------------------------
            --                              VAL DEC
            -- ------------------------------------------------------------------------ 
    
            when mc_fsm_DEC_VAL_READ =>
                -- cteme
                DATA_EN   <= '1';
                DATA_RDWR <= '0';
                -- zapis
                mc_nstate <= mc_fsm_DEC_VAL_WRT;

            -- zde zapisuji
            when mc_fsm_DEC_VAL_WRT =>
                -- zapisujeme
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                -- odecti data pomoci pridruzeneho logickeho
                -- clenu u multiplexeru
                mc_MUX_sel <= "10";
                -- zpatky na zacatek
                mc_nstate <= mc_fsm_BACK;

            -- ------------------------------------------------------------------------
            --                           LOOP START
            -- ------------------------------------------------------------------------ 

            when mc_fsm_LOP_STR =>
                -- posun program counter
                mc_PC_increment <= '1';
                -- dalsi stav
                mc_nstate <= mc_fsm_LOP_STR_2;

            when mc_fsm_LOP_STR_2 =>
                -- povolime cteni
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                -- dalsi stav
                mc_nstate <= mc_fsm_LOP_STR_COND_1;

            -- prvni podminka (if (ram[PTR] == 0))
            when mc_fsm_LOP_STR_COND_1 =>
                -- pokud jsou data rovna nule (tedy nemame vnoreny cyklus)
                if (mc_HLP_data = '1') then
                    -- incrementuj CNT (nova zavorka)
                    mc_CNT_increment <= '1';
                    -- dalsi stav
                    mc_nstate <= mc_fsm_LOP_STR_COND_2;
                else 
                    -- vrat se na zacatek, protoze nemame posledni zavorku
                    mc_nstate <= mc_fsm_FETCH;
                end if;

            -- druha podminka (while (CNT != 0))
            when mc_fsm_LOP_STR_COND_2 =>
                -- pokud counter neni nulovy, mame tedy nejake zavorky
                if (mc_CNT_out = '0') then
                    -- povol cteni z programu
                    CODE_EN <= '1';
                    -- a skoc na samotne vykonavani
                    mc_nstate <= mc_fsm_LOP_STR_WHILE;
                else
                    -- nemame zavorky, skoc na zacatek
                    mc_nstate <= mc_fsm_FETCH;
                end if;

            -- telo cyklu
            when mc_fsm_LOP_STR_WHILE =>
                -- mame levou zavorku
                if (mc_instruction = mc_LOP_STR) then
                    mc_CNT_increment <= '1';
                -- mame pravou zavorku, 
                elsif (mc_instruction = mc_LOP_END) then
                    mc_CNT_decrement <= '1';
                end if;
                -- dalsi stav
                mc_nstate <= mc_fsm_LOP_STR_WHILE_2;

            when mc_fsm_LOP_STR_WHILE_2 =>
                -- zvysime program counter
                mc_PC_increment <= '1';
                -- vratme se na porovnani poctu zavorek
                mc_nstate <= mc_fsm_LOP_STR_COND_2;

            -- ------------------------------------------------------------------------
            --                           LOOP END
            -- ------------------------------------------------------------------------ 

            when mc_fsm_LOP_END =>
                -- cteme
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                -- prechod na prvni podminku
                mc_nstate <= mc_fsm_LOP_END_COND_1;

            -- prvni podminka (if (ram[PTR] == 0))
            when mc_fsm_LOP_END_COND_1 =>
                if (mc_HLP_data = '1') then
                    -- incrementuj PC
                    mc_PC_increment <= '1';
                    -- skoc na zacatek, neni potreba dale resit
                    mc_nstate <= mc_fsm_FETCH;
                else 
                    mc_CNT_increment <= '1';
                    mc_PC_decrement <= '1';
                    ---
                    mc_nstate <= mc_fsm_LOP_END_COND_2;
                end if;

            when mc_fsm_LOP_END_COND_2 =>
                -- pokud counter neni nulovy
                if (mc_CNT_out = '0') then
                    -- povol cteni a skoc na samotne vykonavani
                    CODE_EN <= '1';
                    -- 
                    mc_nstate <= mc_fsm_LOP_END_WHILE;
                else
                    -- nemame zavorky, skoc na zacatek
                    mc_nstate <= mc_fsm_FETCH;
                end if;

            when mc_fsm_LOP_END_WHILE =>
                -- mame pravou zavorku
                if (mc_instruction = mc_LOP_END) then
                    mc_CNT_increment <= '1';
                -- mame levou zavorku, 
                elsif (mc_instruction = mc_LOP_STR) then
                    mc_CNT_decrement <= '1';
                end if;
                -- skoc na druhy while
                mc_nstate <= mc_fsm_LOP_END_WHILE_2;

            when mc_fsm_LOP_END_WHILE_2 =>
                -- druha podminka
                if (mc_CNT_out = '1') then
                    mc_PC_increment <= '1'; 
                    --
                    mc_nstate <= mc_fsm_FETCH;
                -- pracujeme s program counterem
                else
                    mc_PC_decrement <= '1';
                    -- skocime na zacatek cyklu
                    mc_nstate <= mc_fsm_LOP_END_COND_2;
                end if;

            -- ------------------------------------------------------------------------
            --                              BREAK
            -- ------------------------------------------------------------------------ 

            when mc_fsm_LOP_BRK =>
                mc_PC_increment <= '1';
                mc_CNT_increment <= '1';
                -- dalsi stav
                mc_nstate <= mc_fsm_LOP_BRK_COND_1;

            when mc_fsm_LOP_BRK_COND_1 =>
                -- pokud counter neni nulovy
                if (mc_CNT_out = '0') then
                    -- povol cteni a skoc na samotne vykonavani
                    CODE_EN <= '1';
                    mc_nstate <= mc_fsm_LOP_BRK_WHILE;
                else
                    -- nemame break
                    mc_nstate <= mc_fsm_FETCH;
                end if;

            when mc_fsm_LOP_BRK_WHILE =>
                -- mame levou zavorku
                if (mc_instruction = mc_LOP_STR) then
                    mc_CNT_increment <= '1';
                -- mame pravou zavorku, 
                elsif (mc_instruction = mc_LOP_END) then
                    mc_CNT_decrement <= '1';
                end if;
                -- citac
                mc_PC_increment <= '1';
                mc_nstate <= mc_fsm_LOP_BRK_COND_1;

            -- ------------------------------------------------------------------------
            --                              GETCHAR
            -- ------------------------------------------------------------------------ 

            when mc_fsm_GETCHR =>
                IN_REQ <= '1'; -- blokujeme dalsi cinnost procesoru
                -- dokud nemame validni vstup
                if (IN_VLD = '1') then
                    -- multiplexer jede primo, cteme primo ze vstupu
                    mc_MUX_sel <= "00";
                    -- cteme
                    DATA_EN <= '1';
                    DATA_RDWR <= '1';
                    -- vratime zpet hodnotu
                    IN_REQ <= '0';
                    -- zpet
                    mc_nstate <= mc_fsm_BACK;
                else
                    -- skakej tak dlouho zpatky, dokud neni validni vstup
                    mc_nstate <= mc_fsm_GETCHR;
                end if;

            -- ------------------------------------------------------------------------
            --                              PUTCHAR
            -- ------------------------------------------------------------------------

            when mc_fsm_PUTCHR_READ =>
                if (OUT_BUSY = '1') then
                    mc_nstate <= mc_fsm_PUTCHR_READ;
                else
                    -- teoreticky nemusi, nicmene kdyz budeme cist primo
                    -- tak to nebude fungovat, pocitame implicitne
                    DATA_EN <= '1';
                    DATA_RDWR <= '0'; -- cteme z pameti
                    -- muzeme vypsat
                    mc_nstate <= mc_fsm_PUTCHR_WRT;
                end if;

            when mc_fsm_PUTCHR_WRT =>
                -- povolime zapis na LCD
                OUT_WE <= '1'; 
                -- preneseme data
                OUT_DATA <= DATA_RDATA;
                -- zpet
                mc_nstate <= mc_fsm_BACK;

            -- ------------------------------------------------------------------------
            --                Osetreni pripadnych dalsich stavu
            -- ------------------------------------------------------------------------ 
            when others => null;
        end case;
    end process;
end behavioral;
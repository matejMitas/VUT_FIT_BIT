library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
        ROW:    out std_logic_vector (0 to 7);
        LED:    out std_logic_vector (0 to 7);
        RESET:  in std_logic;
        SMCLK:  in std_logic
      );
end ledc8x8;

architecture main of ledc8x8 is

signal my_row:              std_logic_vector(7 downto 0);
signal my_led:              std_logic_vector(7 downto 0);
signal cnt_clk_e:           std_logic_vector(7 downto 0);
signal clk_e:               std_logic;

    -- Sem doplnte definice vnitrnich signalu.

begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

    ---------------------------------
    ---- Lowering the Frequency 
    ---- (We divide the signal to 1/256, to slow it down so 
    ---- we can see the multiplexing happening)
    ---------------------------------

    ---- main process for 
    lower_freq: process(SMCLK, RESET)
    begin
        if (RESET = '1') then
            cnt_clk_e <= "00000000";
        elsif (SMCLK'event) and (SMCLK = '1') then
            cnt_clk_e <= cnt_clk_e + 1;
        end if;
    end process lower_freq;

    ---- condition to reset counter
    clk_e <= '1' when (cnt_clk_e = "11111111") else '0';

    ---------------------------------
    ---- Rotate Rows
    ---- (...)
    ---------------------------------

    rotate_rows: process(SMCLK, RESET, clk_e)
    begin
        if (RESET = '1') then
            my_row <= "10000000";
        elsif (SMCLK'event) and (SMCLK='1') and (clk_e = '1') then
            my_row <= my_row(0) & my_row(7 downto 1);
        end if;
    end process rotate_rows;

    ---------------------------------
    ---- Matrix Decoder
    ---- (...)
    ---------------------------------

    matrix_decoder: process(my_row)
    begin
        case my_row is
            when "10000000" => my_led <= "01100110";
            when "01000000" => my_led <= "00100100";
            when "00100000" => my_led <= "01000010";
            when "00010000" => my_led <= "01100110";
            when "00001000" => my_led <= "01100110";
            when "00000100" => my_led <= "01100110";
            when "00000010" => my_led <= "01100110";
            when "00000001" => my_led <= "01100110";
            when others     => my_led <= "11111111";
        end case;
    end process matrix_decoder;

    ---------------------------------
    ---- Nastav na vystup
    ---- (...)
    ---------------------------------

    ROW <= my_row;
    LED <= my_led;

end main;

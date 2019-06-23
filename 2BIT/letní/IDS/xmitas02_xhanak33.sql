-- Projekt: IDS - FIT VUT
-- PL-SQL skript pro 2. část projektu do IDS
-- Datum: 21.3.2018
-- HanakJiri, xhanak33
-- Matej Mitas, xmitas02

--------------------------------------------------------------------
---
--- Vycisteni tabulek
---
--------------------------------------------------------------------
-- odstran primarni tabulky
DROP TABLE druh CASCADE CONSTRAINTS;
DROP TABLE zeme CASCADE CONSTRAINTS;
DROP TABLE oblasti CASCADE CONSTRAINTS;
DROP TABLE odberatel CASCADE CONSTRAINTS;
DROP TABLE objednavka CASCADE CONSTRAINTS PURGE;
DROP TABLE varka CASCADE CONSTRAINTS;
-- ostran sekundarni tabulky
DROP TABLE druh_ze_zeme CASCADE CONSTRAINTS;
DROP TABLE varka_v_objednavce CASCADE CONSTRAINTS;
DROP TABLE varka_z_druhu CASCADE CONSTRAINTS;
-- tabulky pro generalizaci
DROP TABLE fyzicka_osoba CASCADE CONSTRAINTS;
DROP TABLE pravnicka_osoba CASCADE CONSTRAINTS;

DROP MATERIALIZED VIEW mat_view;
DROP SEQUENCE create_order_seq;
--------------------------------------------------------------------
---
--- Vytvoreni hlavnich tabulek
---
--------------------------------------------------------------------
CREATE TABLE druh (
  -- <<pk>>
  druh_id NVARCHAR2(64) NOT NULL, -- jmeno druhu caje
  -- atributy
  dodavatel NVARCHAR2(255),    -- jmeno dodavatele
  kvalita NUMBER(1),           -- jakost vyrobku (1 - 3)
  pripravy NVARCHAR2(1024),    -- navod na pripravy caje
  louhavani NUMBER(3) NOT NULL, -- doba louhovani v minutach, max 999
  -- kontrola kvalitativni stupnice
  CONSTRAINT CHK_kvalita CHECK (kvalita > 0 AND kvalita <= 3)
);

CREATE TABLE zeme (
  -- <<pk>>
  zeme_id NUMBER(3) NOT NULL,   -- existuje cca 200 zemi, 3 digity staci
  -- atributy
  nazev NVARCHAR2(32),          -- jmeno zeme
  kontinent NVARCHAR2(32),      -- kontinent, v cestine se vsechny vejdou
  -- kontrola poctu zemi
  CONSTRAINT CHK_zeme_id CHECK (zeme_id > 0 AND zeme_id <= 197)
);

CREATE TABLE oblasti (
  -- <<pk>>
  zeme_id NUMBER(3) NOT NULL,   -- existuje cca 200 zemi, 3 digity staci
  -- <<dsc>>
  oznaceni_oblasti NUMBER(2) NOT NULL,   -- kazda zeme musi mit max. 99 oblasti
  -- atributy
  popis NVARCHAR2(1024),       -- 1000 znaku musi stacit
  charakter NVARCHAR2(256)     -- charakteristicke znaky
  -- pridat zde zobecnene omezeni pro zeme
);

CREATE TABLE odberatel (
  -- <<pk>>
  odberatel_id NUMBER NOT NULL,
  -- atributy
  telefon NUMBER(9) NOT NULL,  -- implictne pouze ceska cisla
  email VARCHAR(128) NOT NULL,
  osloveni VARCHAR(64),        -- neni nutne ho mit
  -- adresa
  ulice NVARCHAR2(64) NOT NULL,
  mesto NVARCHAR2(64) NOT NULL,
  psc NVARCHAR2(5) NOT NULL
);

CREATE TABLE objednavka (
  -- <<pk>>
  -- postupne incrementujeme
  objednavka_id NUMBER NOT NULL,
  -- <<fk>>
  odberatel_id NUMBER NOT NULL,
  -- kontrola
  CONSTRAINT CHK_objednavka_id CHECK (objednavka_id > 0)
);

CREATE TABLE varka (
  -- <<pk>>
  varka_id NUMBER NOT NULL,
  cena NUMBER(4) NOT NULL, -- cena za gram
  zbyvajici_mnozstvi NUMBER NOT NULL,
  sleva NUMBER(2),
  -- omezeni
  CONSTRAINT CHK_varka_id CHECK (varka_id > 0),
  CONSTRAINT CHK_mnozstvi CHECK (zbyvajici_mnozstvi > 0),
  CONSTRAINT CHK_sleva CHECK (sleva >= 0 AND sleva <= 99)
);

--------------------------------------------------------------------
---
--- Tabulky pro generalizaci/specializaci
---
--------------------------------------------------------------------
CREATE TABLE fyzicka_osoba (
  -- <<pk>>
  odberatel_id NUMBER NOT NULL,
  -- atributy
  jmeno NVARCHAR2(32) NOT NULL,
  prijmeni NVARCHAR2(16) NOT NULL,
  osobni_telefon NUMBER(9)
);

CREATE TABLE pravnicka_osoba (
  -- <<pk>>
  odberatel_id NUMBER NOT NULL,
  -- atributy
  jmeno_firmy NVARCHAR2(64),
  ic NUMBER(8) NOT NULL,
  dic VARCHAR(12) NOT NULL     -- trigger pro DIČ
);

--------------------------------------------------------------------
---
--- Tabulky pro vztahy M/N
---
--------------------------------------------------------------------

CREATE TABLE druh_ze_zeme (
  druh_id NVARCHAR2(64) NOT NULL,
  zeme_id NUMBER(3) NOT NULL
);

CREATE TABLE varka_z_druhu (
  varka_id NUMBER NOT NULL,
  druh_id NVARCHAR2(64) NOT NULL
);

CREATE TABLE varka_v_objednavce (
  objednavka_id NUMBER NOT NULL,
  varka_id NUMBER NOT NULL,
  mnozstvi NUMBER NOT NULL
);

--------------------------------------------------------------------
---
--- Primarni klice pro hlavni tabulky
---
--------------------------------------------------------------------
ALTER TABLE druh ADD CONSTRAINT PK_druh PRIMARY KEY (druh_id);
ALTER TABLE zeme ADD CONSTRAINT PK_zeme PRIMARY KEY (zeme_id);
ALTER TABLE odberatel ADD CONSTRAINT PK_odberatel PRIMARY KEY (odberatel_id);
ALTER TABLE objednavka ADD CONSTRAINT PK_objednavka PRIMARY KEY (objednavka_id);
ALTER TABLE varka ADD CONSTRAINT PK_varka PRIMARY KEY (varka_id);

--------------------------------------------------------------------
---
--- Cize klice pro vztah 1/M
---
--------------------------------------------------------------------
-- cizi klic pro objednavku
ALTER TABLE objednavka ADD CONSTRAINT FK_objednavka FOREIGN KEY (odberatel_id) REFERENCES odberatel ON DELETE CASCADE;

--------------------------------------------------------------------
---
--- Generalizace (postup #1)
---
--------------------------------------------------------------------
-- primarni
ALTER TABLE fyzicka_osoba ADD CONSTRAINT PK_odberatel_fyz PRIMARY KEY (odberatel_id);
ALTER TABLE pravnicka_osoba ADD CONSTRAINT PK_odberatel_pra PRIMARY KEY (odberatel_id);
-- cizi
ALTER TABLE fyzicka_osoba ADD CONSTRAINT FK_odberatel_fyz FOREIGN KEY (odberatel_id) REFERENCES odberatel ON DELETE CASCADE;;
ALTER TABLE pravnicka_osoba ADD CONSTRAINT FK_odberatel_pra FOREIGN KEY (odberatel_id) REFERENCES odberatel ON DELETE CASCADE;

--------------------------------------------------------------------
---
--- Weak mnozina implemetace
---
--------------------------------------------------------------------
-- vyrobime kompozitni primarni klic
ALTER TABLE oblasti ADD CONSTRAINT PK_oblasti PRIMARY KEY (oznaceni_oblasti, zeme_id);
-- vyrobime cizi klic ukazujici do zemi
ALTER TABLE oblasti ADD CONSTRAINT FK_oblasti FOREIGN KEY (zeme_id) REFERENCES zeme ON DELETE CASCADE;

--------------------------------------------------------------------
---
--- Klice pro vztahove tabulky
---
--------------------------------------------------------------------
-- kompozitni primarni klic (druh ze zeme)
ALTER TABLE druh_ze_zeme ADD CONSTRAINT PK_druh_ze_zeme PRIMARY KEY (druh_id, zeme_id);
-- Vzdy dva cizi klice
ALTER TABLE druh_ze_zeme ADD CONSTRAINT FK_druh_ze_zeme FOREIGN KEY (druh_id) REFERENCES druh ON DELETE CASCADE;
ALTER TABLE druh_ze_zeme ADD CONSTRAINT FK_zeme_pro_druh FOREIGN KEY (zeme_id) REFERENCES zeme ON DELETE CASCADE;

-- kompozitni primarni klic (druh ze zeme)
ALTER TABLE varka_z_druhu ADD CONSTRAINT PK_varka_z_druhu PRIMARY KEY (varka_id, druh_id);
-- Vzdy dva cizi klice
ALTER TABLE varka_z_druhu ADD CONSTRAINT FK_varka_z_druhu FOREIGN KEY (varka_id) REFERENCES varka ON DELETE CASCADE;
ALTER TABLE varka_z_druhu ADD CONSTRAINT FK_druh_do_varky FOREIGN KEY (druh_id) REFERENCES druh ON DELETE CASCADE;

-- kompozitni primarni klic (druh ze zeme)
ALTER TABLE varka_v_objednavce ADD CONSTRAINT PK_varka_v_objednavce PRIMARY KEY (objednavka_id, varka_id);
-- Vzdy dva cizi klice
ALTER TABLE varka_v_objednavce ADD CONSTRAINT FK_varka_v_objednavce FOREIGN KEY (objednavka_id) REFERENCES objednavka ON DELETE CASCADE;
ALTER TABLE varka_v_objednavce ADD CONSTRAINT FK_objednavka_s_varkou FOREIGN KEY (varka_id) REFERENCES varka ON DELETE CASCADE;

--------------------------------------------------------------------
---
--- Aplikacni cast (Triggery a Sekvence)
---
--------------------------------------------------------------------
-- trigger pro DIC
CREATE OR REPLACE TRIGGER dic_validator
  BEFORE INSERT OR UPDATE OF dic ON pravnicka_osoba
  FOR EACH ROW

DECLARE
  -- pro vlastni DIC
  dic VARCHAR(12);
  -- delka DIC
  delka_retezce NUMBER;
  -- spravny kod zeme
  predcisli VARCHAR(2);
  -- kontrola ciselne casti
  ciselna_cast NUMBER;

BEGIN
  -- priradime vlastni string
  dic := :NEW.dic;
  delka_retezce := LENGTH(dic);
  -- mame pozadovanou delku?
  IF (delka_retezce < 10 OR delka_retezce > 12) THEN
    Raise_Application_Error (-20100, 'DIC musi mit predcisli a 8-10 ciselnych znaku');
  END IF;
  -- vysekneme predcisli
  predcisli := SUBSTR(dic, 1, 2);
  -- mame pozadovane predcisli?
  IF (predcisli != 'CZ') THEN
    Raise_Application_Error (-20101, 'Spatne predcisli pro DIC');
  END IF;
  -- vysekneme cisla
  ciselna_cast := TO_NUMBER(SUBSTR(dic, 3, delka_retezce));

EXCEPTION
  WHEN VALUE_ERROR THEN
    Raise_Application_Error (-20102, 'Format DIC nesedni, CZ XXXX XXXX (XX) - X cislo, 8 - 10');
END;
/


CREATE SEQUENCE create_order_seq
  start with 1
  increment by 1;

CREATE TRIGGER create_order_index
  BEFORE INSERT OR UPDATE OF objednavka_id ON objednavka
  FOR EACH ROW
DECLARE
BEGIN
  IF(:new.objednavka_id IS NULL)
  THEN
    :new.objednavka_id := create_order_seq.nextval;
  END IF;
END;
/

--------------------------------------------------------------------
---
--- Naplneni daty
---
--------------------------------------------------------------------
-- vytvor nejake druhy
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Fan', 'Inman s.r.o', '1', 'Připravte si to nejlepší ice tea široko daleko. Nebo použijte čaj Sladká meruňka k prohřátí po podzimní procházce. Nebo mu najděte jiné uplatnění. Vždy si ale buďte jisti, že vás jeho meruňkovo medová kompozice potěší.', '58');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Ulrich', 'Sorilda GmbH', '1', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.', '45');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Chap', 'Hunfredo Ltd', '1', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.', '65');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Perrie', 'Damle Exports Ltda', '1', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.', '38');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Devinne', 'Weisbart AG', '2', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.', '56');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Beitz', 'Priest A.S.', '2', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.', '88');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Eleonore', 'Wun GmbH', '1', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.', '39');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Shanks', 'Pelagi s.r.o', '1', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.', '113');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Herbie', 'Atul GmbH', '1', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.', '94');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Oberg', 'Yetty s.r.o', '2', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.', '20');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Cohe', 'Heins GmbH', '2', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.', '57');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Hettie', 'Berkin Ltda', '2', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.', '199');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Durston', 'Madelon Exports Ltda', '1', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.', '28');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Hutchins', 'Jaenicke Ltd', '2', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.', '138');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Ulu', 'Asel Ltd', '1', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.', '24');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Sellma', 'Izabel Ltd', '2', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.', '105');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Cchaddie', 'Sello GmbH', '1', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.', '145');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Harneen', 'Hyrup Ltd', '2', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.', '40');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Alexandros', 'Rafe Ltda', '1', 'Připravte si to nejlepší ice tea široko daleko. Nebo použijte čaj Sladká meruňka k prohřátí po podzimní procházce. Nebo mu najděte jiné uplatnění. Vždy si ale buďte jisti, že vás jeho meruňkovo medová kompozice potěší.', '120');
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES('Broek', 'Rosmunda GmbH', '1', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.', '18');
-- nejake zeme
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('167', 'Svazijsko', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('124', 'Nizozemsko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('170', 'Španělsko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('76', 'Kapverdy', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('168', 'Sýrie', 'Asie');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('110', 'Mexiko', 'Severní Amerika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('125', 'Norsko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('68', 'Jemen', 'Asie');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('54', 'Haiti', 'Severní Amerika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('153', 'Slovensko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('37', 'Ekvádor', 'Jižní Amerika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('106', 'Maroko', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('144', 'Salvador', 'Severní Amerika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('27', 'Čad', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('180', 'Tunisko', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('57', 'Chorvatsko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('102', 'Malawi', 'Afrika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('56', 'Chile', 'Jižní Amerika');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('136', 'Polsko', 'Evropa');
INSERT INTO zeme (zeme_id, nazev, kontinent) VALUES('66', 'Jamajka', 'Severní Amerika');
-- nejake oblasti
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '0', 'Afrika', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '0', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '0', 'Evropa', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '1', 'Afrika', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '1', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '2', 'Afrika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '3', 'Afrika', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '3', 'Evropa', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '3', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '3', 'Afrika', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '3', 'Asie', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '3', 'Severní Amerika', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('125', '3', 'Evropa', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('68', '3', 'Asie', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '4', 'Afrika', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '5', 'Afrika', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '5', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '5', 'Evropa', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '5', 'Afrika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '5', 'Asie', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '5', 'Severní Amerika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('125', '5', 'Evropa', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('68', '5', 'Asie', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('54', '5', 'Severní Amerika', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '6', 'Afrika', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '6', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '7', 'Afrika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '8', 'Afrika', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '8', 'Evropa', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '8', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '9', 'Afrika', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '9', 'Evropa', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '9', 'Evropa', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '9', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '10', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '11', 'Afrika', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '11', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '11', 'Evropa', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '11', 'Afrika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '11', 'Asie', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '11', 'Severní Amerika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '13', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '13', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '13', 'Evropa', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '13', 'Afrika', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '13', 'Asie', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '14', 'Afrika', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '14', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '14', 'Evropa', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '14', 'Afrika', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '14', 'Asie', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '14', 'Severní Amerika', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('125', '14', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '15', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '15', 'Evropa', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '15', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '15', 'Afrika', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '15', 'Asie', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '16', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '16', 'Evropa', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '16', 'Evropa', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '16', 'Afrika', 'Byli jste někdy v broskvovém sadu? Za teplého odpoledne, když dozrávaly sladké, těžké plody a všude zpívaly cikády? My ano.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '16', 'Asie', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '16', 'Severní Amerika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '18', 'Afrika', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '18', 'Evropa', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '18', 'Evropa', 'Když se řekne Gunpowder, představíme si obvykle zelený čaj stočený do drobných tmavošedých granulek. A teď pozor – podobně vypadá i jeho „Kolega v černém“.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '18', 'Afrika', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '18', 'Asie', 'Jak se vám líbí kombinace svěží zelené a tmavě žluté? Jak se vám líbí sladkost vyšperkovaná svěží kyselkavostí? Jak se vám líbí zelený čaj, papája a vřesové kvítky?');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('110', '18', 'Severní Amerika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('167', '19', 'Afrika', 'Jako když sedíte na louce. Před vámi potok, za vámi vysoké skály. Nadechnete se… a celým tělem prostoupí vůně svěží trávy a horského vzduchu.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('124', '19', 'Evropa', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('170', '19', 'Evropa', 'Druhá sklizeň začíná v Assamu obvykle během května. Čaje, které z ní vzejdou, charakterizuje nádherné plné tělo a zaoblená chuť.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('76', '19', 'Afrika', 'Krásné velké listy, mnoho bílých tipsů, lehká chuť plná meruněk. Jeden z nejoblíbenějších čínských zelených čajů je pojmenovaný po nejjižnějším ledovci na severní polokouli.');
INSERT INTO oblasti (zeme_id, oznaceni_oblasti, popis, charakter) VALUES('168', '19', 'Asie', 'Silná chuť s lehkým kouřovým pohlazením čeká na zvědavé ochutnávače. Zcela nový Gunpowder se v naší nabídce zřejmě dlouho neohřeje.');
-- napojime druhy na zeme
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','167');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','167');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','167');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','167');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','124');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','124');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','124');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','124');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','170');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','170');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Eleonore','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Shanks','76');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Eleonore','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Shanks','168');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','110');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','110');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','110');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','125');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','125');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','125');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','125');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','125');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','68');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','54');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','54');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','54');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','54');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','54');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','153');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','153');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','153');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','153');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','37');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','106');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','106');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','106');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','106');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','106');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','144');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','144');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','144');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','27');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','27');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','27');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','27');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','180');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','180');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','57');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','57');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Eleonore','102');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','56');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Chap','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Perrie','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Devinne','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Beitz','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Eleonore','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Shanks','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Herbie','136');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Fan','66');
INSERT INTO druh_ze_zeme (druh_id, zeme_id) VALUES('Ulrich','66');


-- vytvorime varky
INSERT INTO varka (varka_id, cena, zbyvajici_mnozstvi, sleva) VALUES ('21', '41', '55', '10');
INSERT INTO varka (varka_id, cena, zbyvajici_mnozstvi, sleva) VALUES ('18', '27', '15', '0');
INSERT INTO varka (varka_id, cena, zbyvajici_mnozstvi, sleva) VALUES ('15', '27', '15', '0');
INSERT INTO varka (varka_id, cena, zbyvajici_mnozstvi, sleva) VALUES ('1', '29', '50', '0');
INSERT INTO varka (varka_id, cena, zbyvajici_mnozstvi, sleva) VALUES ('5', '20', '70', '5');

-- pridame druhy do varek
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('21', 'Fan');
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('18', 'Ulrich');
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('18', 'Cohe');
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('15', 'Broek');
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('1', 'Alexandros');
INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES ('5', 'Ulu');
-- vytvorime odberatele
INSERT INTO odberatel (odberatel_id, telefon, email, osloveni, ulice, mesto, psc) VALUES ('1', '123456789', 'mail@mail.com', NULL, 'Lodni 78', 'Praha', '11000');
INSERT INTO odberatel (odberatel_id, telefon, email, osloveni, ulice, mesto, psc) VALUES ('2', '785948247', 'test@mail.com', NULL, 'Vinohradska 11', 'Praha', '11000');
INSERT INTO odberatel (odberatel_id, telefon, email, osloveni, ulice, mesto, psc) VALUES ('3', '785948267', 'test1@mail.com', NULL, 'U hradu 3', 'Olomouc', '12000');
INSERT INTO odberatel (odberatel_id, telefon, email, osloveni, ulice, mesto, psc) VALUES ('4', '787948267', 'test2@mail.com', NULL, 'U hradu 5', 'Brno', '12000');
INSERT INTO odberatel (odberatel_id, telefon, email, osloveni, ulice, mesto, psc) VALUES ('5', '987948267', 'test3@mail.com', NULL, 'U hradu 9', 'Brno', '12000');
-- specializace odberatele
INSERT INTO fyzicka_osoba (odberatel_id, jmeno, prijmeni, osobni_telefon) VALUES ('2', 'Jan', 'Novák', NULL);
INSERT INTO fyzicka_osoba (odberatel_id, jmeno, prijmeni, osobni_telefon) VALUES ('3', 'Josef', 'Novák', NULL);
INSERT INTO fyzicka_osoba (odberatel_id, jmeno, prijmeni, osobni_telefon) VALUES ('4', 'Viktor', 'Novák', NULL);
INSERT INTO fyzicka_osoba (odberatel_id, jmeno, prijmeni, osobni_telefon) VALUES ('5', 'Matej', 'Novák', NULL);
INSERT INTO pravnicka_osoba (odberatel_id, jmeno_firmy, ic, dic) VALUES ('1', 'Hanáčtí páječi s.r.o', '00008180', 'CZ00008180');

-- vytvorime objednavku
INSERT INTO objednavka (odberatel_id) VALUES ('1');
INSERT INTO objednavka (odberatel_id) VALUES ('2');
INSERT INTO objednavka (odberatel_id) VALUES ('3');

-- navazeme objednavku na druh
INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES ('1', '21', '20');
INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES ('2', '18', '15');
INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES ('2', '15', '10');
INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES ('3', '15', '10');
INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES ('3', '21', '10');

COMMIT;


--------------------------------------------------------------------
---
--- Select dotazy
---
--------------------------------------------------------------------

-- Vypise vsechny objednavky, ktere jsou drazsi nez 20 a jsou v ni pouze varky bez slevy
SELECT objednavka_id, SUM(cena) AS Cena_objednavky FROM objednavka
  NATURAL JOIN varka_v_objednavce
  NATURAL JOIN varka
  WHERE sleva = 0
  GROUP BY objednavka_id
  HAVING SUM(cena) > 20
;

-- Vypise vsechny druhy pochazejici z Jizni Ameriky
SELECT DISTINCT druh_id, dodavatel, kvalita, kontinent FROM druh
  NATURAL JOIN druh_ze_zeme
  NATURAL JOIN zeme
  WHERE kontinent = 'Jižní Amerika'
;

-- Vypise zeme z Jizni Ameriky majici vice jak 1 oblast
SELECT nazev FROM zeme
  NATURAL JOIN oblasti
  WHERE kontinent = 'Jižní Amerika'
  GROUP BY nazev
  HAVING COUNT(oznaceni_oblasti) > 1
  ORDER BY nazev ASC
;

-- Vypise vsechny zeme, jejichz zbozi si nekdo objednal
SELECT nazev FROM zeme
  WHERE zeme_id IN (
    SELECT zeme_id FROM objednavka
      NATURAL JOIN varka_v_objednavce
      NATURAL JOIN varka_z_druhu
      NATURAL JOIN druh_ze_zeme
);

-- Vypise objednavky, jejichz odberatele jsou z Prahy
SELECT objednavka_id FROM objednavka
  WHERE EXISTS(
    SELECT objednavka_id FROM objednavka
      NATURAL JOIN odberatel
      WHERE mesto = 'Praha'
);

--------------------------------------------------------------------
---
--- Procedury
---
--------------------------------------------------------------------

-- procedura vypise z objednavky, z jakych varek a v jakem pomeru je slozena
CREATE OR REPLACE PROCEDURE proc1(id_objednavka IN NUMBER) IS
  CURSOR cur IS
    SELECT varka_id, mnozstvi FROM objednavka NATURAL JOIN varka_v_objednavce WHERE objednavka_id = id_objednavka;
  radek cur%ROWTYPE;
  soucet NUMBER;
  BEGIN
    SELECT SUM(mnozstvi) INTO soucet FROM objednavka
      NATURAL JOIN varka_v_objednavce
      WHERE objednavka_id = id_objednavka;
    OPEN cur;
    LOOP
      FETCH cur INTO radek;
      EXIT WHEN cur%NOTFOUND;

      dbms_output.put_line('Varka id:' || radek.varka_id || ' je zastoupena ' || radek.mnozstvi / soucet * 100 || '% v objednavce.');
    END LOOP;
    CLOSE cur;
    EXCEPTION
      WHEN OTHERS THEN
        raise_application_error(-20102, 'Chyba!');
  END;
/

-- procedura vypise vazeny prumer casu louhovani objednavky
CREATE OR REPLACE PROCEDURE proc2(id_objednavka IN NUMBER) IS
  CURSOR cur IS
    SELECT mnozstvi, louhavani FROM objednavka
      NATURAL JOIN varka_v_objednavce
      NATURAL JOIN varka_z_druhu
      NATURAL JOIN druh
      WHERE objednavka_id = id_objednavka;
  radek cur%ROWTYPE;
  citatel NUMBER;
  jmenovatel NUMBER;
  BEGIN
    OPEN cur;
    jmenovatel := 0;
    citatel := 0;
    LOOP
      FETCH cur INTO radek;
      EXIT WHEN cur%NOTFOUND;
      citatel := citatel + radek.louhavani * radek.mnozstvi;
      jmenovatel := jmenovatel + radek.mnozstvi;
    END LOOP;
    CLOSE cur;
    dbms_output.put_line('Prumerny cas louhuvani: ' || citatel / jmenovatel);
    EXCEPTION
      WHEN ZERO_DIVIDE THEN
        dbms_output.put_line('Zadana objednavka neexistuje!');
      WHEN OTHERS THEN
        raise_application_error(-20103, 'Chyba!');
  END;
/

begin
  proc1(2);
end;

begin
  proc2(2);
end;

--------------------------------------------------------------------
---
--- Materializovany pohled
---
--------------------------------------------------------------------

CREATE MATERIALIZED VIEW mat_view
REFRESH ON COMMIT AS
  SELECT druh_id, dodavatel
    FROM druh;

SELECT * FROM mat_view;
INSERT INTO druh (druh_id, dodavatel, kvalita, pripravy, louhavani) VALUES ('Super caj',  'Pepa z Prahy', '1', 'Priprava', '10');
COMMIT;
SELECT * FROM mat_view;

--------------------------------------------------------------------
---
--- Explain plan a index
---
--------------------------------------------------------------------

EXPLAIN PLAN FOR
  SELECT
    odberatel.odberatel_id,
    SUM(cena) AS Cena_objednavky,
    COUNT(objednavka.objednavka_id)
  FROM odberatel
    LEFT JOIN objednavka on objednavka.odberatel_id = odberatel.odberatel_id
    LEFT JOIN varka_v_objednavce ON objednavka.objednavka_id = varka_v_objednavce.objednavka_id
    LEFT JOIN varka ON varka.varka_id = varka_v_objednavce.varka_id
  WHERE mesto = 'Praha'
  GROUP BY odberatel.odberatel_id
;

SELECT * FROM TABLE (DBMS_XPLAN.display);

CREATE INDEX index1 ON odberatel (mesto);

EXPLAIN PLAN FOR
  SELECT
    odberatel.odberatel_id,
    SUM(cena) AS Cena_objednavky,
    COUNT(objednavka.objednavka_id)
  FROM odberatel
    LEFT JOIN objednavka on objednavka.odberatel_id = odberatel.odberatel_id
    LEFT JOIN varka_v_objednavce ON objednavka.objednavka_id = varka_v_objednavce.objednavka_id
    LEFT JOIN varka ON varka.varka_id = varka_v_objednavce.varka_id
  WHERE mesto = 'Praha'
  GROUP BY odberatel.odberatel_id
;

SELECT * FROM TABLE (DBMS_XPLAN.display);

--------------------------------------------------------------------
---
--- Opravneni
---
--------------------------------------------------------------------

GRANT ALL ON oblasti TO xhanak33;
GRANT ALL ON zeme TO xhanak33;
GRANT ALL ON druh TO xhanak33;
GRANT ALL ON varka TO xhanak33;
GRANT ALL ON objednavka TO xhanak33;
GRANT ALL ON odberatel TO xhanak33;
GRANT ALL ON fyzicka_osoba TO xhanak33;
GRANT ALL ON pravnicka_osoba TO xhanak33;
GRANT ALL ON mat_view TO xhanak33;
GRANT EXECUTE ON proc1 TO xhanak33;
GRANT EXECUTE ON proc2 TO xhanak33;
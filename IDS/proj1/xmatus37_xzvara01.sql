DROP SEQUENCE TicketSeq;
DROP SEQUENCE EventSeq;
DROP SEQUENCE StageSeq;
DROP SEQUENCE BandSeq;
DROP SEQUENCE BandMemberSeq;
DROP TABLE TicketOrder;
DROP TABLE Ticket;
DROP TABLE Concert;
DROP TABLE Customer;
DROP TABLE BandPerformance;
DROP TABLE Stage;
DROP TABLE Festival;
DROP TABLE BandMember;
DROP TABLE Band;

-- Customer table
    -- login does not start with number, consists of alphanumeric values, has at least 4 characters
    -- for simplicity password is stored directly as character array (without hashing)
    -- the longest postal code currently in use in the world is 10 digits long
CREATE TABLE Customer (
    login       VARCHAR(64) PRIMARY KEY,
    passwd      VARCHAR(64) NOT NULL,
    firstname   VARCHAR(128),
    lastname    VARCHAR(128),
    birthday    DATE,
    uState      VARCHAR(128),
    city        VARCHAR(128),
    street      VARCHAR(128),
    postalCode  VARCHAR(10),
    CHECK (REGEXP_LIKE(login, '^[a-zA-Z][a-zA-Z0-9]{4,}$'))
);


CREATE SEQUENCE BandSeq
    START WITH 1
    INCREMENT BY 1;

-- Table Band
CREATE TABLE Band (
    bandID INTEGER DEFAULT ON NULL BandSeq.NEXTVAL PRIMARY KEY,
    bandName VARCHAR(64) NOT NULL,
    created DATE NOT NULL,
    genre VARCHAR(32),
    CHECK (bandID > 0)
);

-- Event tables
    -- generalization with separate tables for each specialized entity (type 2 in presentation)
        -- tables Concert and Festival to represent specialization
    -- other options:
        -- creating single table with column "type", would result in many NULL columns
        --    (e.g. we do not need to store interprets or capacity for each festival)
        -- creating "super" table called Events was an option, but the table itself does not
        --      have any meaning (every event is either Festival or Concert)

-- Festivals and Concerts could have same PRIMARY KEYS for some columns
-- which could cause some problems when assigning Ticket to an event
-- hence we used same sequence generator for IDs
CREATE SEQUENCE EventSeq
START WITH 100
INCREMENT BY 1;

CREATE TABLE Festival (
    festivalID   INTEGER DEFAULT ON NULL EventSeq.NEXTVAL PRIMARY KEY,
    eventDate    DATE NOT NULL,
    eventPlace   VARCHAR(128) NOT NULL,
    eventCountry VARCHAR(128) NOT NULL,
    eventName    VARCHAR(128) NOT NULL,
    festRepeat   SMALLINT DEFAULT 0,
    CHECK (festivalID > 0 AND festRepeat > 0)
);

CREATE TABLE Concert (
    concertID    INTEGER DEFAULT ON NULL EventSeq.NEXTVAL PRIMARY KEY,
    eventDate    DATE NOT NULL,
    eventPlace   VARCHAR(128) NOT NULL,
    eventCountry VARCHAR(128) NOT NULL,
    eventName    VARCHAR(128) NOT NULL,
    concCapacity INTEGER,
    bandID       INTEGER,
    CONSTRAINT BandIDFK FOREIGN KEY (bandID) REFERENCES Band ON DELETE CASCADE,
    CHECK (concertID > 0 AND concCapacity > 0)
);

CREATE SEQUENCE TicketSeq
START WITH 1
INCREMENT BY 1;

-- Tickets table
    -- ticketPrice - price of single ticket (in euros) of given type to an event
    -- ticketCount - amount of tickets availiable to an event
CREATE TABLE Ticket (
    ticketID    INTEGER DEFAULT ON NULL TicketSeq.NEXTVAL PRIMARY KEY,
    ticketType  VARCHAR(20) DEFAULT ON NULL 'STAND',
    ticketPrice SMALLINT NOT NULL,
    ticketCount INTEGER NOT NULL,
    ticketEventType VARCHAR(1) NOT NULL,
    ticketEvent INTEGER,
    CHECK (ticketID > 0 AND ticketPrice > 0 AND ticketCount > 0),
    CHECK (ticketType in ('VIP', 'STAND', 'SIT', 'ONEDAY', 'MULTIDAY')),
    CHECK (ticketEventType in ('F', 'C'))
);

-- Code taken from https://stackoverflow.com/questions/68704320/reference-to-one-or-another-table-in-oracle-sql?fbclid=IwAR12uHB7rmGtVZPp63xDhDXou8yJXk2YOjt-0kRxl6PX3PBEF06QVV7Awzw
-- Ticket contains only one reference either to Concert or Festival table
-- which depends on ticketEventType
ALTER TABLE Ticket
  ADD (ConcertID  INVISIBLE AS (CASE WHEN ticketEventType = 'C' THEN ticketEvent END) REFERENCES Concert ON DELETE CASCADE,
       FestivalID INVISIBLE AS (CASE WHEN ticketEventType = 'F' THEN ticketEvent END) REFERENCES Festival ON DELETE CASCADE);

-- Table TickerOrder
    -- contains each order created by user
    -- ! user can only create one order to an event with particular ticket type !
CREATE TABLE TicketOrder (
    login VARCHAR(64),
    ticketID  INTEGER,
    amount    INTEGER NOT NULL,
    CONSTRAINT TicketOrderPK PRIMARY KEY (login, ticketID),
    CONSTRAINT userLoginFK FOREIGN KEY (login) REFERENCES Customer ON DELETE CASCADE,
    CONSTRAINT ticketIDFK FOREIGN KEY (ticketID) REFERENCES Ticket ON DELETE CASCADE
);

CREATE SEQUENCE StageSeq
    START WITH 1
    INCREMENT BY 1;

-- Table Stage
    -- contains information about stage
    -- stageID, capacity and area are > 0
CREATE TABLE Stage (
    stageID INTEGER DEFAULT ON NULL StageSeq.NEXTVAL PRIMARY KEY,
    stageName VARCHAR(64) NOT NULL,
    crowdCapacity INTEGER NOT NULL,
    bandCapacity INTEGER NOT NULL,
    area INTEGER NOT NULL,
    festivalID INTEGER,
    headlinerID INTEGER,
    CONSTRAINT FestivalIDFK FOREIGN KEY (festivalID) REFERENCES Festival,
    CONSTRAINT HeadlinerIDFK FOREIGN KEY (headlinerID) REFERENCES Band,
    CHECK (stageID > 0 AND crowdCapacity > 0 AND bandCapacity > 0 AND area > 0)
);

CREATE SEQUENCE BandMemberSeq
    START WITH 1
    INCREMENT BY 1;

-- Table BandMember
CREATE TABLE BandMember (
    memberID INTEGER DEFAULT ON NULL BandMemberSeq.NEXTVAL PRIMARY KEY,
    firstName VARCHAR(32) NOT NULL,
    lastName VARCHAR(32),
    bandID INTEGER,
    CONSTRAINT BandID3FK FOREIGN KEY (bandID) REFERENCES Band,
    CHECK (memberID > 0)
);

-- Table BandPerformance
CREATE TABLE BandPerformance (
    stageID INTEGER,
    bandID INTEGER,
    hour INTEGER NOT NULL,
    CONSTRAINT BandPerformancePK PRIMARY KEY (stageID, bandID),
    CONSTRAINT StageIDFK FOREIGN KEY (stageID) REFERENCES Stage ON DELETE CASCADE,
    CONSTRAINT BandID4FK FOREIGN KEY (bandID) REFERENCES Band ON DELETE CASCADE,
    CHECK (hour >= 0 AND hour < 24)
);

-- Adding customers
INSERT INTO Customer VALUES('matej01', 'passwd123', 'Matej', 'Dlhý', TO_DATE('4/3/1993', 'DD/MM/YY'), 'Slovakia', 'Košice', 'Jaltská 17', '04022');
INSERT INTO Customer VALUES('p3t3r', 'kolobezka', 'Peter', 'Krátky', TO_DATE('19.8.2001', 'DD/MM/YY'), 'Slovakia', 'Bratislava', 'Brodská 23', '84104');
INSERT INTO Customer VALUES('tadeas', 'as123mkas', 'Tadeas', 'Vrtký', TO_DATE('2.1.1992', 'DD/MM/YY'), 'Czech Republic', 'Brno', 'Purkyňova 93', '61200');
INSERT INTO Customer VALUES('j4mes4', 'OOPLanguage', 'James', 'Jameson', TO_DATE('28.2.1975', 'DD/MM/YY'), 'USA', 'New York', '5th Avenue 42', '10016-7103');
INSERT INTO Customer VALUES('anonym0us', 'HahahHAa', NULL, NULL, NULL, NULL, NULL, NULL, NULL);
INSERT INTO Customer VALUES('AveRAge', 'IDontEvenKnow', 'Michael', 'Average', NULL, 'Poland', NULL, NULL, NULL);
INSERT INTO Customer VALUES('Speedro', 'Speedro123', 'Pedro', 'Ramirez', TO_DATE('03/23/1989', 'MM/DD/YY'), 'Spain', 'Madrid', 'Preciados 23', '28013');

-- Adding festivals
INSERT INTO Festival VALUES(DEFAULT, TO_DATE('7/7/2022', 'DD/MM/YY'),  'Letisko Trenčín', 'Slovakia', 'Pohoda', 24);
INSERT INTO Festival VALUES(DEFAULT, TO_DATE('15/6/2022', 'DD/MM/YY'), 'Hradec Králové', 'Czech Republic', 'Rock For People', 25);
INSERT INTO Festival VALUES(DEFAULT, TO_DATE('23/7/2024', 'DD/MM/YY'), 'Maracanã', 'Brazil', 'Rock In Rio', 4);
INSERT INTO Festival VALUES(DEFAULT, TO_DATE('15/4/2023', 'DD/MM/YY'), 'Empire Polo Club', 'California', 'Coachella', 20);
INSERT INTO Festival VALUES(DEFAULT, TO_DATE('15/6/2022', 'DD/MM/YY'), 'Žilina', 'Slovakia', 'NewFoundFest', 2);

-- Add bands
-- bandID, bandName, created, genre
INSERT INTO Band VALUES(DEFAULT, 'Trivium', TO_DATE('5/6/1999', 'DD/MM/YYYY'), 'Heavy metal');
INSERT INTO Band VALUES(DEFAULT, 'Guns n Roses', TO_DATE('12/2/1985', 'DD/MM/YYYY'), 'Hard rock');
INSERT INTO Band VALUES(DEFAULT, 'Bleed from Within', TO_DATE('24/9/2005', 'DD/MM/YYYY'), 'Metalcore');
INSERT INTO Band VALUES(DEFAULT, 'Whitechapel', TO_DATE('20/12/2006', 'DD/MM/YYYY'), 'Death metal');
INSERT INTO Band VALUES(DEFAULT, 'Apocalyptica', TO_DATE('20/12/2006', 'DD/MM/YYYY'), 'Symphonic metal');
INSERT INTO Band VALUES(DEFAULT, 'Deftones', TO_DATE('20/12/2006', 'DD/MM/YYYY'), 'Alternative metal');

-- Adding concerts
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('7/7/2022', 'DD/MM/YY'), 'Letiště Praha Letňany', 'Czech Republic', 'Guns n Roses', 80000, 2);
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('18/6/2022', 'DD/MM/YY'), 'Amfiteáter Rača', 'Slovakia', 'Apocalyptica', 40000, 1);
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('12/1/2023', 'DD/MM/YY'), 'O2 ACADEMY GLASGOW', 'United Kingdom', 'Trivium', 50000, 3);
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('20/4/2022', 'DD/MM/YY'), 'The Greek Theatre', 'California', 'Deftones', 25000, 4);
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('7/7/2022', 'DD/MM/YY'), 'Letiště Praha Letňany', 'Czech Republic', 'Bleed from Within', 80000, 3);
INSERT INTO Concert VALUES(DEFAULT, TO_DATE('21/4/2023', 'DD/MM/YY'), 'Letiště Praha Letňany', 'Czech Republic', 'Guns n Roses', 80000, 2);

-- Adding tickets
-- Trivium
INSERT INTO Ticket VALUES(DEFAULT, DEFAULT, 90, 30000, 'C', 106);
INSERT INTO Ticket VALUES(DEFAULT, 'SIT', 140, 25000, 'C', 106);
INSERT INTO Ticket VALUES(DEFAULT, 'VIP', 290, 5000, 'C', 106);
-- Guns n Roses
INSERT INTO Ticket VALUES(DEFAULT, DEFAULT, 95, 10000, 'C', 105);
INSERT INTO Ticket VALUES(DEFAULT, 'SIT', 160, 7500, 'C', 105);
INSERT INTO Ticket VALUES(DEFAULT, 'VIP', 300, 2500, 'C', 105);
-- Rock for people
INSERT INTO Ticket VALUES(DEFAULT, DEFAULT, 100, 80000, 'F', 101);
INSERT INTO Ticket VALUES(DEFAULT, 'VIP', 500, 5000, 'F', 101);
INSERT INTO Ticket VALUES(DEFAULT, 'ONEDAY', 80, 100000, 'F', 101);
INSERT INTO Ticket VALUES(DEFAULT, 'MULTIDAY', 300, 40000, 'F', 101);
-- Pohoda
INSERT INTO Ticket VALUES(DEFAULT, DEFAULT, 100, 50000, 'F', 100);
INSERT INTO Ticket VALUES(DEFAULT, 'VIP', 520, 2000, 'F', 100);
INSERT INTO Ticket VALUES(DEFAULT, 'ONEDAY', 65, 80000, 'F', 100);
INSERT INTO Ticket VALUES(DEFAULT, 'MULTIDAY', 130, 20000, 'F', 100);

-- Add tickerOrders
INSERT INTO ticketOrder VALUES('matej01', 1, 4);
INSERT INTO ticketOrder VALUES('matej01', 2, 2);
INSERT INTO ticketOrder VALUES('tadeas', 7, 10);
INSERT INTO ticketOrder VALUES('j4mes4', 8, 2);
INSERT INTO ticketOrder VALUES('j4mes4', 7, 3);
INSERT INTO ticketOrder VALUES('j4mes4', 6, 2);
INSERT INTO ticketOrder VALUES('Speedro', 9, 3);

-- Add band members
-- memberID, firstName, lastName, bandID
-- Trivium
INSERT INTO BandMember VALUES(DEFAULT, 'Matt', 'Heafy', 1);
INSERT INTO BandMember VALUES(DEFAULT, 'Corey', 'Beaulieu', 1);
INSERT INTO BandMember VALUES(DEFAULT, 'Paolo', 'Gregoletto', 1);
INSERT INTO BandMember VALUES(DEFAULT, 'Alex', 'Bent', 1);
-- GnR
INSERT INTO BandMember VALUES(DEFAULT, 'Axl', 'Rose', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Duff', 'McKagan', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Slash', '', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Dizzy', 'Reed', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Richard', 'Fortus', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Frank', 'Ferrer', 2);
INSERT INTO BandMember VALUES(DEFAULT, 'Melissa', 'Reese', 2);
-- Bleed from Within
INSERT INTO BandMember VALUES(DEFAULT, 'Scott', 'Kenedy', 3);
INSERT INTO BandMember VALUES(DEFAULT, 'Ali', 'Richardson', 3);
INSERT INTO BandMember VALUES(DEFAULT, 'Craig', 'Gowans', 3);
INSERT INTO BandMember VALUES(DEFAULT, 'Davie', 'Provan', 3);
INSERT INTO BandMember VALUES(DEFAULT, 'Steven', 'Jones', 3);
-- Whitechapel
INSERT INTO BandMember VALUES(DEFAULT, 'Phil', 'Bozeman', 4);
INSERT INTO BandMember VALUES(DEFAULT, 'Ben', 'Savage', 4);
INSERT INTO BandMember VALUES(DEFAULT, 'Gabe', 'Crisp', 4);
INSERT INTO BandMember VALUES(DEFAULT, 'Alex', 'Wade', 4);
INSERT INTO BandMember VALUES(DEFAULT, 'Zach', 'Householder', 4);
-- Apocalyptica
INSERT INTO BandMember VALUES(DEFAULT, 'Eicca', 'Toppinen', 5);
INSERT INTO BandMember VALUES(DEFAULT, 'Peritu', 'Kivilaakso', 5);
INSERT INTO BandMember VALUES(DEFAULT, 'Paavo', 'Lötjönen', 5);
INSERT INTO BandMember VALUES(DEFAULT, 'Mikko', 'Siren', 5);
-- Deftones
INSERT INTO BandMember VALUES(DEFAULT, 'Stephen', 'Carpenter', 6);
INSERT INTO BandMember VALUES(DEFAULT, 'Abe', 'Cunningham', 6);
INSERT INTO BandMember VALUES(DEFAULT, 'Chino', 'Moreno', 6);
INSERT INTO BandMember VALUES(DEFAULT, 'Frank', 'Delgado', 6);

-- Add stages
-- stage for festival with a headliner band
-- stageID, stageName, crowdCapacity, bandCapacity, area, festivalID, bandID
INSERT INTO Stage VALUES(DEFAULT, 'Letisko Trenčín', 20000, 4, 200, 100, 1);
INSERT INTO Stage VALUES(DEFAULT, 'Hradec Kralove', 30000, 4, 400, 101, 4);
INSERT INTO Stage VALUES(DEFAULT, 'Maracana', 50000, 7, 500, 102, 3);
INSERT INTO Stage VALUES(DEFAULT, 'Empire Polo Club', 40000, 2, 200, 103, 3);
INSERT INTO Stage VALUES(DEFAULT, 'Male podium', 10000, 4, 100, 100, 6);
INSERT INTO Stage VALUES(DEFAULT, 'Hradec Kralove MP', 15000, 2, 220, 101, 5);
INSERT INTO Stage VALUES(DEFAULT, 'El Poerto', 20000, 7, 150, 102, 2);
INSERT INTO Stage VALUES(DEFAULT, 'Empire Exclusive', 10000, 1, 170, 103, 1);

-- Add performances
-- stageID, bandID, time
INSERT INTO BandPerformance VALUES(2, 1, 22);
INSERT INTO BandPerformance VALUES(3, 2, 20);
INSERT INTO BandPerformance VALUES(3, 3, 19);
INSERT INTO BandPerformance VALUES(4, 3, 23);
INSERT INTO BandPerformance VALUES(1, 1, 0);
INSERT INTO BandPerformance VALUES(1, 4, 19);
INSERT INTO BandPerformance VALUES(2, 4, 22);
INSERT INTO BandPerformance VALUES(3, 1, 22);

-- grant access rights to another user
GRANT ALL ON BAND TO XZVARA01;
GRANT ALL ON BANDMEMBER TO XZVARA01;
GRANT ALL ON BANDPERFORMANCE TO XZVARA01;
GRANT ALL ON CONCERT TO XZVARA01;
GRANT ALL ON CUSTOMER TO XZVARA01;
GRANT ALL ON FESTIVAL TO XZVARA01;
GRANT ALL ON STAGE TO XZVARA01;
GRANT ALL ON TICKET TO XZVARA01;
GRANT ALL ON TICKETORDER TO XZVARA01;

-- Trigger: User changes his login and ticketOrder needs to be updated
CREATE OR REPLACE TRIGGER update_login
    AFTER UPDATE OF login ON Customer
    FOR EACH ROW
BEGIN
    UPDATE ticketOrder SET login = :new.login
    WHERE login = :old.login;
END;
/

-- State before: Customer with login matej01, ticketOrders with same login
-- State after: Customer with changed login to matejTestUpdate and ticketOrder with new login
UPDATE Customer
SET login = 'matejTestUpdate'
WHERE login = 'matej01';

-- Trigger: If user tries to buy tickets for concert or festival exceeding the amount of total tickets
--          raise exception
CREATE OR REPLACE TRIGGER ticket_exceeding_total_amount
    BEFORE INSERT OR UPDATE OF amount ON ticketOrder
    FOR EACH ROW
DECLARE
    v_total     NUMBER;
    v_newTotal  NUMBER;
BEGIN
    -- Sum all amounts of ordered tickets for given ticketID
    SELECT COALESCE(SUM(amount), 0) -- If newTotal is NULL replace it with 0
        INTO v_newTotal
        FROM ticketOrder
        WHERE ticketID = :new.ticketID;
    -- Get total number of tickets available
    SELECT ticketCount
        INTO v_total
        FROM Ticket
        WHERE ticketID = :new.ticketID;
    IF v_total < v_newTotal+:new.amount THEN
        raise_application_error(-20001, 'Exceeded amount of availiable tickets');
    END IF;
END;
/
    
-- Try to insert new ticketOrder exceeding amount of all available tickets
INSERT INTO ticketOrder VALUES('p3t3r', 14, 50000);
-- Same but with already existing ticketOrder for given ticket type
INSERT INTO ticketOrder VALUES('tadeas', 14, 19000);
INSERT INTO ticketOrder VALUES('AveRAge', 14, 2000);

-- Procedure: 
-- Print out summary of all musical events with ticket orders for each event
CREATE OR REPLACE PROCEDURE events_overview IS
    CURSOR concert_cursor IS SELECT * FROM Concert; -- explicit cursor
    concert_record Concert%ROWTYPE;
    festival_record Festival%ROWTYPE;
BEGIN
    DBMS_OUTPUT.PUT_LINE('Concerts:');
    FOR concert_record IN concert_cursor LOOP
        single_event_overview(concert_record.concertID, concert_record.eventName);
    END LOOP;
    
    DBMS_OUTPUT.PUT_LINE('Festivals:');
    FOR festival_record IN (SELECT * FROM Festival) LOOP -- implicit cursor
        single_event_overview(festival_record.festivalID, festival_record.eventName);
    END LOOP;
END;
/

-- helper procedure to print out results 
CREATE OR REPLACE PROCEDURE single_event_overview(eventID NUMBER, eventName VARCHAR) IS
    v_event_all_tickets NUMBER;
    v_event_ordered_tickets NUMBER;
    v_occupancy_percentage NUMBER;
BEGIN
     SELECT COALESCE(SUM(ticketCount), 0)
        INTO v_event_all_tickets 
        FROM Ticket 
        WHERE ticketEvent = eventID;
    SELECT COALESCE(SUM(AMOUNT), 0)
        INTO v_event_ordered_tickets
        FROM Ticket NATURAL JOIN ticketOrder
        WHERE ticketEvent = eventID;
    v_occupancy_percentage := v_event_ordered_tickets / v_event_all_tickets * 100;
    DBMS_OUTPUT.PUT_LINE('Name: ' || eventName || ', Total tickets: ' || v_event_all_tickets || ', Ordered Tickets: ' || v_event_ordered_tickets || ' ' || CEIL(v_occupancy_percentage) || '%');   
EXCEPTION
    WHEN ZERO_DIVIDE THEN
        DBMS_OUTPUT.PUT_LINE('Event ' || eventName || ' has no available tickets');
END;
/

-- insert more ticket orders
INSERT INTO Ticket VALUES (15, 'STAND',  100, 50000, 'C', 108);
INSERT INTO Ticket VALUES (16, 'STAND',  100, 80000, 'C', 109);
INSERT INTO Ticket VALUES (17, 'ONEDAY', 100, 100000, 'F', 102);
INSERT INTO ticketOrder VALUES('j4mes4', 1, 20590);
INSERT INTO ticketOrder VALUES('j4mes4', 4, 9030);
INSERT INTO ticketOrder VALUES('j4mes4', 9, 83030);
INSERT INTO ticketOrder VALUES('j4mes4', 13, 50301);
INSERT INTO ticketOrder VALUES('j4mes4', 15, 45631);
INSERT INTO ticketOrder VALUES('j4mes4', 17, 64212);

CALL events_overview();
    
-- Procedure: Write name of bands playing on given festival
CREATE OR REPLACE PROCEDURE festival_bands(festivalName VARCHAR) IS
    v_festival_id Festival.festivalID%TYPE;
    v_performance_record BandPerformance%ROWTYPE;
    v_band_name Band.BandName%TYPE;
    v_stage_record Stage%ROWTYPE;
BEGIN
    SELECT festivalID
        INTO v_festival_id
        FROM Festival
        WHERE Festival.eventName = festivalName;
    DBMS_OUTPUT.PUT_LINE('Festival: ' || festivalName);
    FOR v_stage_record IN (SELECT * FROM Stage WHERE Stage.festivalID = v_festival_id) LOOP
        DBMS_OUTPUT.PUT_LINE('Stage: ' || v_stage_record.StageName);
        FOR v_performance_record IN (SELECT * FROM BandPerformance WHERE StageID = v_stage_record.StageID) LOOP
            SELECT BandName
                INTO v_band_name
                FROM Band
                WHERE BandID = v_performance_record.BandID;
            DBMS_OUTPUT.PUT_LINE(v_band_name);
         END LOOP;
    END LOOP;
END;
/

CALL festival_bands('Pohoda');
CALL festival_bands('Rock For People');
CALL festival_bands('Rock In Rio');

-- explain plan + index
EXPLAIN PLAN FOR
SELECT eventName, eventPlace, ticketCountTotal
FROM Concert JOIN
     (SELECT ticketEvent, SUM(TicketCount) ticketCountTotal
      FROM Ticket
      GROUP BY ticketEvent) ON (ticketEvent=concertID)
UNION
SELECT eventName, eventPlace, ticketCountTotal
FROM Festival JOIN
     (SELECT ticketEvent, SUM(ticketCount) ticketCountTotal
      FROM Ticket
      GROUP BY ticketEvent) ON (ticketEvent=FestivalID)
ORDER BY ticketCountTotal DESC;
SELECT * FROM TABLE(DBMS_XPLAN.display);

CREATE INDEX index_ticket ON Ticket(ticketEvent);

EXPLAIN PLAN FOR
    SELECT eventName, eventPlace, ticketCountTotal
    FROM Concert JOIN
        (SELECT ticketEvent, SUM(TicketCount) ticketCountTotal
         FROM Ticket
         GROUP BY ticketEvent) ON (ticketEvent=concertID)
    UNION
    SELECT eventName, eventPlace, ticketCountTotal
    FROM Festival JOIN
        (SELECT ticketEvent, SUM(ticketCount) ticketCountTotal
         FROM Ticket
         GROUP BY ticketEvent) ON (ticketEvent=FestivalID)
    ORDER BY ticketCountTotal DESC;
SELECT * FROM TABLE(DBMS_XPLAN.display);

DROP INDEX index_ticket;

-- materialized view
CREATE MATERIALIZED VIEW mat_view
CACHE
BUILD IMMEDIATE
REFRESH ON COMMIT AS
    SELECT XMATUS37.CONCERT.EVENTNAME, XMATUS37.CONCERT.EVENTPLACE
    FROM XMATUS37.CONCERT
    ORDER BY XMATUS37.CONCERT.CONCCAPACITY;

-- revoke access rights from another user
REVOKE ALL ON BAND FROM XZVARA01;
REVOKE ALL ON BANDMEMBER FROM XZVARA01;
REVOKE ALL ON BANDPERFORMANCE FROM XZVARA01;
REVOKE ALL ON CONCERT FROM XZVARA01;
REVOKE ALL ON CUSTOMER FROM XZVARA01;
REVOKE ALL ON FESTIVAL FROM XZVARA01;
REVOKE ALL ON STAGE FROM XZVARA01;
REVOKE ALL ON TICKET FROM XZVARA01;
REVOKE ALL ON TICKETORDER FROM XZVARA01;

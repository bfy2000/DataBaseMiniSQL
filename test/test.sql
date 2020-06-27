CREATE TABLE student(
    age INT,
    stuid CHAR(20) UNIQUE,
    name CHAR(30)
);

CREATE INDEX studentid ON student(stuid);


DROP INDEX someindex;


SELECT * FROM student;

insert into student values (22,'abc','abc');
CREATE TABLE student(
    age INT,
    stuid CHAR(20) UNIQUE,
    name CHAR(30)
);

CREATE INDEX studentid ON student(stuid);


DROP INDEX studentid;


SELECT * FROM student;

insert into student values (22,'abc','abc');


create table student(
    sid char(8),
    sname char(16) UNIQUE,
    age int,
    weight float,
    primary key (sid)
    );


 create index snameindex on student(sname);
insert into student values ('0001','1',18,50.0);
insert into student values ('0002','2',19,51.0);
insert into student values ('0003','3',18,50.0);
insert into student values ('0004','4',18,50.0);
insert into student values ('0005','5',18,50.0);
insert into student values ('0006','6',18,50.0);
insert into student values ('0007','7',18,50.0);
insert into student values ('0008','8',18,50.0);
insert into student values ('0009','9',18,50.0);
insert into student values ('0010','10',18,50.0);
insert into student values ('0011','11',18,50.0);
insert into student values ('0012','12',18,50.0);
insert into student values ('0013','13',18,50.0);
insert into student values ('0014','14',18,50.0);
insert into student values ('0015','15',18,50.0);
insert into student values ('0016','16',17,50.0);
insert into student values ('0017','17',18,50.0);
insert into student values ('0018','18',19,50.0);
insert into student values ('0019','19',18,50.0);
insert into student values ('0020','20',18,50.0);
insert into student values ('0021','21',18,52.0);
insert into student values ('0022','22',18,50.0);
insert into student values ('0023','23',18,50.0);
insert into student values ('0024','24',18,50.0);
insert into student values ('0025','25',18,50.0);
insert into student values ('0026','26',18,50.0);
insert into student values ('0027','27',18,50.0);
insert into student values ('0028','28',18,50.0);
insert into student values ('0029','29',18,51.0);
insert into student values ('0030','30',18,50.0);
insert into student values ('0031','31',18,50.0);
insert into student values ('0032','32',18,50.0);
insert into student values ('0033','33',18,50.0);
insert into student values ('0034','34',18,50.0);
insert into student values ('0035','35',18,50.0);
insert into student values ('0036','36',18,50.0);
insert into student values ('0037','37',18,50.0);
insert into student values ('0038','38',17,50.0);
insert into student values ('0039','39',18,50.0);
insert into student values ('0040','40',18,52.0);
insert into student values ('0041','41',18,50.0);
insert into student values ('0042','42',18,50.0);
insert into student values ('0043','43',18,50.0);
insert into student values ('0044','44',18,50.0);
insert into student values ('0045','45',18,50.0);
insert into student values ('0046','46',18,50.0);
insert into student values ('0047','47',18,50.0);
insert into student values ('0048','48',18,50.0);
insert into student values ('0049','49',18,50.0);
insert into student values ('0050','50',19,51.0);
insert into student values ('0051','51',18,50.0);
insert into student values ('0052','52',18,50.0);
insert into student values ('0053','53',18,50.0);
insert into student values ('0054','54',18,50.0);
insert into student values ('0055','55',18,50.0);
insert into student values ('0056','56',18,50.0);
insert into student values ('0057','57',17,50.0);
insert into student values ('0058','58',18,50.0);
insert into student values ('0059','59',18,50.0);
insert into student values ('0060','60',18,50.0);
insert into student values ('0061','61',18,51.0);
insert into student values ('0062','62',18,50.0);
insert into student values ('0063','63',18,50.0);
insert into student values ('0064','64',18,51.0);
insert into student values ('0065','65',18,50.0);
insert into student values ('0066','66',18,50.0);
insert into student values ('0067','67',18,50.0);
insert into student values ('0068','68',18,50.0);
insert into student values ('0069','69',18,50.0);
insert into student values ('0070','70',17,50.0);
insert into student values ('0071','71',18,50.0);
insert into student values ('0072','72',18,49.0);
insert into student values ('0073','73',18,50.0);
insert into student values ('0074','74',18,51.0);
insert into student values ('0075','75',18,50.0);
insert into student values ('0076','76',18,50.0);
insert into student values ('0077','77',18,50.0);
insert into student values ('0078','78',18,50.0);
insert into student values ('0079','79',18,50.0);
insert into student values ('0080','80',19,51.0);
insert into student values ('0081','81',18,50.0);
insert into student values ('0082','82',18,50.0);
insert into student values ('0083','83',18,50.0);
insert into student values ('0084','84',18,53.0);
insert into student values ('0085','85',18,50.0);
insert into student values ('0086','86',17,50.0);
insert into student values ('0087','87',18,50.0);
insert into student values ('0088','88',18,50.0);
insert into student values ('0089','89',18,50.0);
insert into student values ('0090','90',18,50.0);
insert into student values ('0091','91',18,50.0);
insert into student values ('0092','92',18,50.0);
insert into student values ('0093','93',18,50.0);
insert into student values ('0094','94',18,50.0);
insert into student values ('0095','95',18,52.0);
insert into student values ('0096','96',18,50.0);
insert into student values ('0097','97',18,50.0);
insert into student values ('0098','98',18,50.0);
insert into student values ('0099','99',18,50.0);
insert into student values ('0100','100',18,50.0);


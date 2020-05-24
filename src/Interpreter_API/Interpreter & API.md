- [x] insert
```sql
insert into student values (‘12345678’,’wy’,22,’M’);
```

- [X] create table
```sql
create table student (
    sno char(8),
    sname char(16) unique,
    sage int,
    sgender char (1),
    primary key ( sno )
);
```

- [X]
```mysql
select * from student;
select * from student where sno = ‘88888888’;
select * from student where sage > 20 and sgender = ‘F’;
```

- [X] drop index
```sql
drop index stunameidx;
```

- [X]
```sql
create index stunameidx on student ( sname );
```

- [X] drop table
```sql
drop table student;
```

- [X] quit 
```sql
quit
```

- [X] 
```sql
execfile xxx;
```
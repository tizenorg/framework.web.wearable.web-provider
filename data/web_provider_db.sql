BEGIN TRANSACTION;      
CREATE TABLE LiveboxInfo (
    box_id TEXT not null,
    app_id TEXT not null,
    box_type TEXT not null,
    auto_launch INT,
    mouse_event INT,
    pd_fast_open INT,
    PRIMARY KEY (box_id) ,
CHECK(1) );
COMMIT;

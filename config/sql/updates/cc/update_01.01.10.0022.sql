------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：李孝成
--修改时间：2013.3.19
--修改补丁号：0022
--修改内容说明：增加VM的usb表
------------------------------------------------------------------------------------
CREATE TABLE vm_usb ( 
	vid                  numeric( 19 ) NOT NULL,
	vendor_id            integer NOT NULL,
	product_id           integer NOT NULL,
	name                 varchar( 128 ),
	CONSTRAINT idx_vm_usb_pool PRIMARY KEY ( vid, vendor_id, product_id),
	CONSTRAINT fk_vm_usb_pool FOREIGN KEY ( vid ) REFERENCES vm_pool( vid ) ON DELETE CASCADE 
 );


 ------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0022', 'lixch',now(),'add vm_usb_pool table');
------------------------------------------------------------------------------------


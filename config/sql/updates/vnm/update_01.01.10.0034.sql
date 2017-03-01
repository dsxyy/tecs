------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张文剑
--修改时间：2012.5.28
--修改补丁号：0000
--修改内容说明：本文件是数据库schema升级脚本的示例范本
------------------------------------------------------------------------------------
--请在此加入vnm表结构, 存贮过程修改的语句，视图的修改请直接编辑tc/cc/vnm_view.sql：
--

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');

-- 611003977573 【TECS】网络平面segmentid范围包含了1，Share SegmentID还有1，重复了 
--
-- 
------------------------------------------------------------------------------------

 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0034', 'tecs-vnm_test',now(),'first version');
  

  
  CREATE OR REPLACE FUNCTION pf_net_query_netplane_vmshare_segments(in_uuid character varying)
  RETURNS character varying AS
$BODY$ 
DECLARE is_exist integer; 
    retchar character varying;    
    tmpchar character varying;
    tmpcharB character varying;
    tmpcharE character varying;
    netplaneid numeric(19,0);
    msegmentrangelists RECORD;
    i integer := 0;
    aindex integer := 0; 
    index integer := 1;
    segmentpre integer := 1;
    segmentbegin integer := 1;
BEGIN retchar := '';
    /* netplane id*/
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid; 
   SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid; 
   IF is_exist <= 0 THEN 
       return retchar; 
   END IF; 
   i := 1;        

   is_exist = 0;
   FOR msegmentrangelists IN SELECT segment_begin,segment_end  FROM segment_range 
        WHERE netplane_id = netplaneid ORDER BY segment_begin LOOP
        
       segmentbegin = msegmentrangelists.segment_begin;

       is_exist = 1;
       
       /* 第一条记录 */
       IF segmentpre <= 1 THEN 
           IF segmentpre < segmentbegin THEN 
              IF segmentpre < (segmentbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  tmpcharE := pf_net_integer_to_char(segmentbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF segmentpre = (segmentbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF;
       END IF;
       IF segmentpre > 1 THEN 
           IF segmentpre < segmentbegin THEN 
              IF segmentpre < (segmentbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(segmentpre+1);
                  tmpcharE := pf_net_integer_to_char(segmentbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF segmentpre = (segmentbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(segmentpre+1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF; 
       END IF;

       segmentpre = msegmentrangelists.segment_end;
   END LOOP;

   /*判断segment range end 是否是16000000*/
   /* 当没有配置segment range 的时候，需考虑边界值1 */
   IF segmentpre = 1 AND is_exist =0 THEN 
       segmentpre = 0;
   END IF;

   IF segmentpre < 16000000 THEN 
       IF segmentpre < (16000000-1) THEN 
           tmpcharB := pf_net_integer_to_char(segmentpre+1);
           tmpcharE := to_char((16000000),'99999999');
           retchar = retchar || tmpcharB || ' ~' || tmpcharE  ;
       END IF;
       IF segmentpre = (16000000-1) THEN 
           tmpcharB := pf_net_integer_to_char(segmentpre+1);
           retchar = retchar || tmpcharB  ;
       END IF;
   END IF;
        
   /* RAISE WARNING  ' retchar --> %', retchar; */
   return retchar;
END; 
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  


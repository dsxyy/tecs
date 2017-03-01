<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<title>TECS</title>
</head>

<body>
    <div>
        <ul class="breadcrumb">
            <li>
                <a href="#">我的虚拟机</a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#">虚拟机</a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#">拟制中</a>
            </li>
        </ul>
    </div>
    <div align="right">
        <a class="btn" href="javascript:void(0);"><i class="icon-file"></i>创建</a> 
        <a class="btn" href="javascript:void(0);"><i class="icon-step-forward"></i>部署</a> 
        <a class="btn" href="javascript:void(0);"><i class="icon-remove"></i>删除</a>
    </div> 
    <div>
        <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" style="width:100%;word-break:break-all">
            <thead> 
                <tr>
                    <th align="left" class="span1"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                    <th align="left">ID</th>
                    <th align="left">名称</th>
                    <th align="left">CPU个数</th>
                    <th align="left">单核能力</th>
                    <th align="left">内存大小</th>
                    <th align="left">硬盘和镜像大小</th>
                    <th align="left">工程</th>
                    <th align="left"></th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td align="left"><INPUT type="checkbox" value="" /></th>
                    <td align="left"><a>1</a></td>
                    <td align="left">vm</td>
                    <td align="left">1个</td>
                    <td align="left">1(TCU)</td>
                    <td align="left">1204MB</td>
                    <td align="left">10GB</td>
                    <td align="left">defalut</td>
                    <td align="left"><a>克隆</a>&nbsp;|&nbsp;<a>互斥</a></td>
                </tr>
                <tr>
                    <td align="left"><INPUT type="checkbox" value="" /></th>
                    <td align="left"><a>2</a></td>
                    <td align="left">vm</td>
                    <td align="left">1个</td>
                    <td align="left">1(TCU)</td>
                    <td align="left">1204MB</td>
                    <td align="left">10GB</td>
                    <td align="left">defalut</td>
                    <td align="left"><a>克隆</a>&nbsp;|&nbsp;<a>互斥</a></td>
                </tr>
            </tbody>
        </table>
        <div><label id="lbvrsummary"></label></div>
    </div>
	<!-- external javascript
	================================================== -->
	<!-- Placed at the end of the document so the pages load faster -->
</body>
</html>

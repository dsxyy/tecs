function offset(node){ 
    var x = node.offsetLeft; 
    var y = node.offsetTop; 
    var w = node.offsetWidth; 
    var h = node.offsetHeight; 
    var parent = node.offsetParent; 
    while (parent != null){ 
        x += parent.offsetLeft; 
        y += parent.offsetTop; 
        parent = parent.offsetParent; 
    } 
    if(w==0){ 
        w+=parseInt(node.currentStyle.width); 
        w+=parseInt(node.currentStyle.paddingRight); 
        w+=parseInt(node.currentStyle.paddingLeft); 
        w+=parseInt(node.currentStyle.borderWidth) * 2; 
    } 
    if(h==0){ 
        h+=parseInt(node.currentStyle.height); 
        h+=parseInt(node.currentStyle.paddingTop); 
        h+=parseInt(node.currentStyle.paddingBottom); 
        h+=parseInt(node.currentStyle.borderWidth) * 2; 
    } 
    return {x: x, y: y, w: w, h: h}; 
} 

function OrgNode(){ 
    this.Text=null; 
    this.Link=null; 
    this.Description=""; 
    this.BoxWidth=null; 
    this.BoxHeight=null; 
    this.parentNode=null; 
    this.NodeGroupId=null; //同一层的级别序号,从零开始 
    this.NodeOrderId=null; //同一级中的序号,从零开始 
    this.TopLine=null; 
    this.BottomLine=null; 
    this.Depth=null; 
    this.Top=null; 
    this.Left=null; 
    this.Type=null; 
    this.Nodes=[]; 
    this.customParam=[]; //节点自定义参数 
    var This=this; 
	
    this.Nodes.Add=function(OrgNode_){ 
        OrgNode_.parentNode=This; 
        This.Nodes[This.Nodes.length]=OrgNode_; 
    } 
    this.Box=null; 
    this.Templet=null; 
    this.Id="OrgNode_"+ GetRandomId(20); //随机id

    this.inIt= function(){ 		
        if(this.inIted==true)return; 
		
		//添加给页面中的div其id是showbackup
        var tempDiv=document.createElement("DIV"); 
        $('#showbackup').append(tempDiv); 
		
        //替换模板中的数据
        var tempHTML=this.Templet; 
        tempHTML=tempHTML.replace("{Id}", this.Id); 
        tempHTML=tempHTML.replace("{Text}", this.Text); 
        tempHTML=tempHTML.replace("{Time}", this.Createtime); 
        tempHTML=tempHTML.replace("{Description}", this.Description); 
        tempHTML=tempHTML.replace("{Target}", this.Target); 		
        for(var Param_ in  this.customParam){ 
            tempHTML=tempHTML.replace("{"+ Param_ +"}", this.customParam[Param_]); 
        } 
        tempDiv.innerHTML=tempHTML; 
		
        this.Box= document.getElementById(this.Id);
        this.Width=offset(this.Box).w; 
        this.Height=offset(this.Box).h; 
        this.inIted=true; 
    } 
	
    function GetRandomId(n_){ 
        var litter="abcdefghijklmnopqrstuvwxyz" 
        litter+=litter.toUpperCase() 
        litter+="1234567890"; 
        var idRnd=""; 
        for(var i=1; i<=n_; i++){ 
            idRnd+=litter.substr((0 + Math.round(Math.random() * (litter.length - 0))), 1) 
        } 
        return idRnd; 
    } 
} 

function OrgShow(OrgNode_){ 
    this.LineSize=2; 
    this.LineColor="#000000"; 
    this.IntervalWidth=100; 
    this.IntervalHeight=50;
    this.Top=0; 
    this.Left=0; 
    this.Depth=0; 
    this.Nodes=[]; 
    this.DepthGroup=[]; //this.DepthGroup[n].Nodes 层深节点集合 
    var This=this;
    this.BoxWidth=null; 
    this.BoxHeight=null; 
    this.BoxTemplet=null; 
    this.ShowType=null; 

    this.Run=function(){ 	
        BoxInit(OrgNode_); 
        GetDepth(OrgNode_); 
        SetDepthsWidth()//设置层宽度

        //*************************** 确定left位置
        for(var n=1; n<=this.Depth; n++){	       
            var tempTop=this.Left + GetDepthHeightToRoot(n); 
            var tempNodes=this.DepthGroup[n].Nodes; 
            for(var m=0; m<tempNodes.length; m++){ 
                tempNodes[m].Left=tempTop; 
            } 
        } 
	
        //***************************确定top位置 
       for(var n=this.Depth; n>=1; n--){
            var DepthNodes=this.DepthGroup[n].Nodes; 
            if(n==this.Depth){ 
                for(var m=0; m<DepthNodes.length; m++){ 
                    if(m==0){ 
                        DepthNodes[m].Top=0; 
                    }else{ 
                        DepthNodes[m].Top=DepthNodes[m-1].Top + DepthNodes[m-1].Height + this.IntervalHeight; 
                    } 
                }	
				
            }else{ 		
                for(var m=0; m<DepthNodes.length; m++){
                    if(DepthNodes[m].Nodes.length!=0){ 
			  	    var tempNodeTop_=DepthNodes[m].Nodes[0].Top ;
                        DepthNodes[m].Top = tempNodeTop_; 
                    } 
                } 
                for(var m=0; m<DepthNodes.length; m++){ 
                    if(DepthNodes[m].Top==null){//没有下级节点的， 
                        SetLeftByDepthNode(DepthNodes, m, "LTR"); 			
                    } 
                } 
			
                for(var m=1; m<DepthNodes.length; m++){//修正错误位置 
                    var ErrDistance=this.IntervalHeight - (DepthNodes[m].Top - DepthNodes[m-1].Top - DepthNodes[m-1].Height); 
                    if(ErrDistance>0){ 
                        for(var u_=m; u_<DepthNodes.length; u_++){ 
                            AmendNodeTop(DepthNodes[u_], ErrDistance); 
                        } 
                    } 
                } 
            } 
        }
	
        SetDepthGroupHeight();//设置群组宽度 
	
        var MaxTopValue=this.Nodes[0].Top; 
        for(var n=1; n<this.Nodes.length; n++){//取得最小距离 
            if(MaxTopValue>this.Nodes[n].Top){ 
                MaxTopValue=this.Nodes[n].Top; 
            } 
        } 
	
        MaxTopValue=(-MaxTopValue) + this.Top; 
        for(var n=0; n<this.Nodes.length; n++){//重新设置距离 
            this.Nodes[n].Top+=MaxTopValue; 
            this.Nodes[n].Box.style.top=this.Nodes[n].Top + "px" 
            this.Nodes[n].Box.style.left=this.Nodes[n].Left + "px" 
        } 
	
	
        //*************************** 节点连线
        for(var n=1; n<=this.Depth; n++){//设置横线条 
            var tempNodes=this.DepthGroup[n].Nodes; 
            for(var m=0; m<tempNodes.length; m++){ 
                if(n!=this.Depth){//设置节点右线条 
                    if(tempNodes[m].Nodes.length!=0){ 					
					    var tempLineHeight = this.IntervalWidth/2 ; 
					    var tempLineTop = tempNodes[m].Top + (tempNodes[m].Height / 2); 
					    var tempLineLeft = tempNodes[m].Left + tempNodes[m].Width; 
                        var tempBottomLine=new CreateLine(1, tempLineTop, tempLineLeft, tempLineHeight, this.LineSize, this.LineColor, "LineBottom_"+ tempNodes[m].Id); 
                        tempNodes[m].BottomLine=tempBottomLine.Line; 
                    } 
                } 
						
                if(n!=1){//设置节点左线条 				
                    var tempLineLeft = 	tempNodes[m].Left - this.IntervalWidth /2; 			
                    var tempLineHeight= this.IntervalWidth/ 2; 				
                    var tempLineTop=tempNodes[m].Top + (tempNodes[m].Height / 2); 	 				
								
                    if(this.DepthGroup[tempNodes[m].Depth].NodeGroups[tempNodes[m].NodeGroupId].length==1){//如果只有一个节点 				
                        tempNodes[m].parentNode.BottomLine.style.width=(this.IntervalWidth)+"px"; 
                    }else{ 
                        var temptopLine=new CreateLine(1, tempLineTop, tempLineLeft, tempLineHeight, this.LineSize, this.LineColor, "LineTop_"+ tempNodes[m].Id); 
                        tempNodes[m].TopLine=temptopLine.Line; 
                    }
					
                    //箭头
                    for(var i=3; i>0; i--){  
						drawDot((tempNodes[m].Left-i), (tempLineTop-i));  
						drawDot((tempNodes[m].Left-i), (tempLineTop+i));  						   
                    }                        					   
                } 
            } 
        } 

        for(var n=2; n<=this.Depth; n++){//设置竖线条 
            var tempNodeGroups_=this.DepthGroup[n].NodeGroups; 
            for(var m=0; m<tempNodeGroups_.length; m++){ 
                if(tempNodeGroups_[m].length!=1){ 				
				    var tempLineTop = tempNodeGroups_[m][0].Top + (tempNodeGroups_[m][0].Height / 2);
				    var tempLineLeft = tempNodeGroups_[m][0].Left - ((this.IntervalWidth - this.LineSize) / 2) - this.LineSize; 				
                    var tempLineWidth=tempNodeGroups_[m].Height - (tempNodeGroups_[m][0].Height / 2) + this.LineSize; 
                    tempLineWidth-=(tempNodeGroups_[m][tempNodeGroups_[m].length-1].Height / 2); 
                    var tempGroupLine=new CreateLine(2, tempLineTop, tempLineLeft, tempLineWidth, this.LineSize, this.LineColor, "LineGroup_"+ tempNodeGroups_[m][0].Id); 
                } 
            } 
        } 


        //*************************** 节点模板
        function GetBoxTemplet(){
            if(This.BoxTemplet!=null){			
			    return This.BoxTemplet; 
            }				
            var TempletStr="<div id=\"{Id}\" data=\"{Text}\" tarvalue=\"{Target}\" class='span2' style=\"padding:5px 5px 5px 5px;clear:left;float:left;text-align:center;position:absolute  ;" 
            if(This.ShowType==2){
                TempletStr+="\" title=\"{Description}\" ><a href=\"javascript:void(0);\" ><span><img src='img/image32.png' /></span><div class='span8 row' ><label data=\"{Time}\" style='whiteSpace:nowrap'></label></div></a></div>"; 
	        }						
            return TempletStr; 
        } 
		
        function BoxInit(OrgObj_){
            OrgObj_.Templet=GetBoxTemplet(); 
            OrgObj_.BoxWidth=This.BoxWidth; 
            OrgObj_.BoxHeight=This.BoxHeight; 	
            OrgObj_.inIt(); 
            if(OrgObj_.Nodes.length!=0){ 
                for(var n=0; n<OrgObj_.Nodes.length; n++){ 
                    BoxInit(OrgObj_.Nodes[n]); 
                } 
            } 
        } 
	
        function AmendNodeTop(Node_, ErrDistance_){//修正错误位置 
            Node_.Top=Node_.Top + ErrDistance_; 
            if(Node_.Nodes.length!=0){ 
                for(var n=0; n<Node_.Nodes.length; n++){ 
                    AmendNodeTop(Node_.Nodes[n], ErrDistance_); 
                } 
            } 
        }
		
    } 
	
//************************************************************************************************* 	
    function GetGroupWidthByNode(Node_){//根据群组中任一节点，取得群组宽度 
        var tempNodesGroup_=This.DepthGroup[Node_.Depth].NodeGroups[Node_.NodeGroupId]; 
        var tempGroupWidth_=tempNodesGroup_[tempNodesGroup_.length-1].Top - tempNodesGroup_[0].Top; 
        tempGroupWidth_+=tempNodesGroup_[tempNodesGroup_.length-1].Height; 
        return tempGroupWidth_; 
    } 

    function SetLeftByDepthNode(DepthNodes_, NodeId, Type){ 
        if(Type=="LTR"&&NodeId==DepthNodes_.length-1){ 
            SetLeftByDepthNode(DepthNodes_, NodeId, "RTL"); 
            return; 
        }
        if(Type=="RTL"&&NodeId==0){ 
            SetLeftByDepthNode(DepthNodes_, NodeId, "LTR"); 
            return; 
        } 
        var FindIndex=null; 
        if(Type=="LTR"){ 
            for(var r_=NodeId+1; r_<DepthNodes_.length; r_++){ 
                if(DepthNodes_[r_].Top!=null){ 
                    FindIndex=r_; 
                    break; 
                } 
            } 
            if(FindIndex==null){ 
                SetLeftByDepthNode(DepthNodes_, NodeId, "RTL"); 
                return; 
            }else{ 
                for(var r_=FindIndex-1; r_>=NodeId; r_--){ 
                    DepthNodes_[r_].Top=DepthNodes_[r_+1].Top - This.IntervalHeight - DepthNodes_[r_].Height; 
                } 
            } 
        } 
        if(Type=="RTL"){ 
            for(var r_=NodeId-1; r_>=0; r_--){ 
                if(DepthNodes_[r_].Top!=null){ 
                    FindIndex=r_; 
                    break; 
                } 
            } 
            if(FindIndex==null){ 
                SetLeftByDepthNode(DepthNodes_, NodeId, "LTR"); 
                return; 
            }else{ 
                for(var r_=FindIndex+1; r_<=NodeId; r_++){ 
                    DepthNodes_[r_].Top=DepthNodes_[r_-1].Top + This.IntervalHeight + DepthNodes_[r_-1].Height; 
                } 
            } 
        } 
    } 

    function GetDepthHeightToRoot(DepthId){//取得至根节点的深度 
        var tempHeight_=0; 
        for(var x_=DepthId; x_>=1; x_--){ 
            tempHeight_+=This.DepthGroup[x_].Width; 
        } 
        tempHeight_+=This.IntervalWidth * (DepthId - 1); 
        tempHeight_-=This.DepthGroup[DepthId].Width; 
        return tempHeight_; 
    } 

    function SetDepthGroupHeight(){//设置层高度
        for(var n_=1; n_<=This.Depth; n_++){ 
            var tempNodeGroups=This.DepthGroup[n_].NodeGroups; 
            for(var m_=0; m_<tempNodeGroups.length; m_++){ 
                tempNodeGroups[m_].Height=GetGroupWidthByNode(tempNodeGroups[m_][0]); 
            } 
        } 
    } 

    function SetDepthsWidth(){//设置层宽度 
        for(var n_=1; n_<=This.Depth; n_++){ 
            var tempNodes_=This.DepthGroup[n_].Nodes; 
            var MaxHeight=0; 
            for(var m_=0; m_<tempNodes_.length; m_++){ 
                if(tempNodes_[m_].Width>MaxHeight){ 
                    MaxHeight=tempNodes_[m_].Width; 
                } 
            } 
            This.DepthGroup[n_].Width=MaxHeight; 
        } 
    } 

    function GetDepth(OrgObj){//取得层深,层群集 
        This.Nodes[This.Nodes.length]=OrgObj; 
        OrgObj.Depth=(This.Depth==0)?This.Depth+1:OrgObj.parentNode.Depth+1; 
        This.Depth=(OrgObj.Depth>This.Depth)?OrgObj.Depth:This.Depth; 
        if(typeof(This.DepthGroup[OrgObj.Depth])!="object"){ 
            This.DepthGroup[OrgObj.Depth]=[]; 
            This.DepthGroup[OrgObj.Depth].Nodes=[]; 
            This.DepthGroup[OrgObj.Depth].NodeGroups=[]; 
        } 
        This.DepthGroup[OrgObj.Depth].Nodes[This.DepthGroup[OrgObj.Depth].Nodes.length]=OrgObj; 
        if(OrgObj.Depth==1){ 
            This.DepthGroup[OrgObj.Depth].NodeGroups[0]=[]; 
            This.DepthGroup[OrgObj.Depth].NodeGroups[0][0]=OrgObj; 
            OrgObj.NodeGroupId=0; 
            OrgObj.NodeOrderId=0; 
        }else{ 
            if(This.DepthGroup[OrgObj.Depth].NodeGroups.length==0){ 
                This.DepthGroup[OrgObj.Depth].NodeGroups[0]=[]; 
                This.DepthGroup[OrgObj.Depth].NodeGroups[0][0]=OrgObj; 
                OrgObj.NodeGroupId=0; 
                OrgObj.NodeOrderId=0; 
            }else{ 
                var GroupsLength=This.DepthGroup[OrgObj.Depth].NodeGroups.length; 
                var GroupNodesLength=This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength-1].length; 
                if(OrgObj.parentNode==This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength-1][GroupNodesLength-1].parentNode){ 
                    This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength-1][GroupNodesLength]=OrgObj; 
                    OrgObj.NodeGroupId=GroupsLength-1; 
                    OrgObj.NodeOrderId=GroupNodesLength; 
                }else{ 
                    if(typeof(This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength])!="object"){ 
                       This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength]=[]; 
                    } 
                    GroupNodesLength=This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength].length; 
                    This.DepthGroup[OrgObj.Depth].NodeGroups[GroupsLength][GroupNodesLength]=OrgObj; 
                    OrgObj.NodeGroupId=GroupsLength; 
                    OrgObj.NodeOrderId=GroupNodesLength; 
                } 
            } 
        } 

        if(OrgObj.Nodes.length!=0){ 
            for(var n=0; n<OrgObj.Nodes.length; n++){ 
                GetDepth(OrgObj.Nodes[n]); 
            } 
        } 
    } 

    function CreateLine(type_, top_, left_, long_, size_, color_, id_){    //连接线
        this.Type=type_; 
        top_=top_+""; 
        left_=left_+""; 
        long_=long_+""; 
        this.Top=(top_.substr(top_.length-2).toLowerCase()!="px")?top_+"px":top_; 
        this.Left=(left_.substr(left_.length-2).toLowerCase()!="px")?left_+"px":left_; 
        this.Long=(long_.substr(long_.length-2).toLowerCase()!="px")?long_+"px":long_; 
        this.Size=(size_==undefined)?"1px":size_+""; 
        this.Id=(id_==undefined)?null:id_; 
        this.Size=(this.Size.substr(this.Size.length-2).toLowerCase()!="px")?this.Size+"px":this.Size; 
        this.Color=(color_==undefined)?"#000000":color_; 
        this.Line=document.createElement("DIV"); 
        document.getElementById('showbackup').appendChild(this.Line); 
        this.Line.innerText="x"; 
        this.Line.style.position="absolute"; 
        this.Line.style.top=this.Top; 
        this.Line.style.left=this.Left; 
        this.Line.style.overflow="hidden"; 
        if(this.Type==1){ 
            this.Line.style.borderTopColor=this.Color; 
            this.Line.style.borderTopWidth=this.Size; 
            this.Line.style.borderTopStyle="solid"; 
            this.Line.style.width=this.Long; 
            this.Line.style.height="0px"; 
        }else{ 
            this.Line.style.borderLeftColor=this.Color; 
            this.Line.style.borderLeftWidth=this.Size; 
            this.Line.style.borderLeftStyle="solid"; 
            this.Line.style.height=this.Long; 
            this.Line.style.width="0px"; 
        } 
        if(this.Id!=null)this.Line.id=this.Id; 
    } 

	function drawDot(top,left){
        var dot = document.createElement('div');  
        dot.style.left =  top + 'px';  
        dot.style.top = left + 'px';  
        dot.style.height = '2px';  
        dot.style.width = '2px';  
        dot.style.position = 'absolute';  
        dot.style.fontSize = '1px';  
        dot.style.backgroundColor = '#cccccc';  
        dot.style.overflow = "hidden";  
        document.getElementById('showbackup').appendChild(dot); 
        dot = null; 		
	}	
} 
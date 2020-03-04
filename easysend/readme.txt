2019/12/25  lora通信协议
/*************lora无线通信协议*****************/
lora模块->控制器   格式：0xAA 0x55 + 校验和 + 长度 + 自身地址 + 目标地址 +命令 + 数据(状态+数据)
#define EVENTPORT    0x0D      //事件上传
#define HEARTBEAT    0x0E      //心跳包
#define WOGLN          0x0A    //注册/自检


控制器->lora模块         格式：0xAA 0x55 + 校验和 + 长度 + 自身地址 + 目标地址 +命令
#define  AGEING          0X10   //老化模式
#define  WWRID          0x00   //ACK


/***************感烟模块与lora模块通信命令*****************/
烟感->lora模块   格式：0xAA + 校验和 +长度 + 命令 + 烟感数据（7字节）注：长度是从命令开始算的
#define EVENTPORT    0x0D      //事件上传
#define HEARTBEAT    0x0E      //心跳包
#define WOGLN               0x0A    //注册/自检

lora模块 ->烟感   格式：0xAA +拓展命令(0x06) +命令 + 类型（定长4个字节）
AA 06 00 00  退出老化模式
AA 06 00 01  进入老化模式




/****************ID烧写工装与lora模块的通信命令***********************/
工装->lora模块  格式：0xAA + 校验和 + 长度 + 命令 +数据（目前最大13个字节）
#define ID_WR     0x01       //写ID命令
#define ID_RD      0x0C      //读ID 命令
#define ID_DEL    0x0F       //擦除ID命令

lora模块->工装   格式：0xAA + 拓展命令（0x08） + 命令 +ID号(定长16个字节)
#define  ID_WRFAILD    0x02   //写ID失败（ID已存在）
#define  ID_WORK        0x03   //写ID成功
#define  ID_ANS           0x0C   //读ID命令回复  

/***********老化工装和lora模块的通信协议****************/
和控制器的一样
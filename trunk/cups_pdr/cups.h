#define DRIVER_VERSION                "1.03"
#define PATH_CUPS_PDR                 "?:\\OS2\\DLL\\CUPS.PDR"
#define CUPS_DLL                      "CUPS.PDR"

#define NO_ERROR                  0
#define ERROR_MORE_DATA           234
#define ERROR_INSUFFICIENT_BUFFER 122

#define PORT_ID_1                 5001
#define PORT_DESC_1               5002
#define PORT_ID_2                 5003
#define PORT_DESC_2               5004
#define PORT_ID_3                 5005
#define PORT_DESC_3               5006
#define PORT_ID_4                 5007
#define PORT_DESC_4               5008
#define PORT_ID_5                 5009
#define PORT_DESC_5               5010
#define PORT_ID_6                 5011
#define PORT_DESC_6               5012
#define PORT_ID_7                 5013
#define PORT_DESC_7               5014
#define PORT_ID_8                 5015
#define PORT_DESC_8               5016
#define PORT_ID_9                 5017
#define PORT_DESC_9               5018
#define PORT_ID_10                5019
#define PORT_DESC_10              5020
#define PORT_ID_11                5021
#define PORT_DESC_11              5022
#define PORT_ID_12                5023
#define PORT_DESC_12              5024
#define PORT_ID_13                5025
#define PORT_DESC_13              5026
#define PORT_ID_14                5027
#define PORT_DESC_14              5028
#define PORT_ID_15                5029
#define PORT_DESC_15              5030
#define PORT_ID_16                5031
#define PORT_DESC_16              5032
#define PORT_ID_17                5033
#define PORT_DESC_17              5034
#define PORT_ID_18                5035
#define PORT_DESC_18              5036
#define PORT_ID_19                5037
#define PORT_DESC_19              5038
#define PORT_ID_20                5039
#define PORT_DESC_20              5040
#define PORT_ID_21                5041
#define PORT_DESC_21              5042
#define PORT_ID_22                5043
#define PORT_DESC_22              5044
#define PORT_ID_23                5045
#define PORT_DESC_23              5046
#define PORT_ID_24                5047
#define PORT_DESC_24              5048
#define PORT_ID_25                5049
#define PORT_DESC_25              5050
#define PORT_ID_26                5051
#define PORT_DESC_26              5052
#define PORT_ID_27                5053
#define PORT_DESC_27              5054
#define PORT_ID_28                5055
#define PORT_DESC_28              5056
#define PORT_ID_29                5057
#define PORT_DESC_29              5058
#define PORT_ID_30                5059
#define PORT_DESC_30              5060
#define PORT_ID_31                5061
#define PORT_DESC_31              5062
#define PORT_ID_32                5063
#define PORT_DESC_32              5064
#define PORT_ID_33                5065
#define PORT_DESC_33              5066
#define PORT_ID_34                5067
#define PORT_DESC_34              5068
#define PORT_ID_35                5069
#define PORT_DESC_35              5070
#define PORT_ID_36                5071
#define PORT_DESC_36              5072
#define PORT_ID_37                5073
#define PORT_DESC_37              5074
#define PORT_ID_38                5075
#define PORT_DESC_38              5076
#define PORT_ID_39                5077
#define PORT_DESC_39              5078
#define PORT_ID_40                5079
#define PORT_DESC_40              5080
#define PORT_ID_41                5081
#define PORT_DESC_41              5082
#define PORT_ID_42                5083
#define PORT_DESC_42              5084
#define PORT_ID_43                5085
#define PORT_DESC_43              5086
#define PORT_ID_44                5087
#define PORT_DESC_44              5088
#define PORT_ID_45                5089
#define PORT_DESC_45              5090
#define PORT_ID_46                5091
#define PORT_DESC_46              5092
#define PORT_ID_47                5093
#define PORT_DESC_47              5094
#define PORT_ID_48                5095
#define PORT_DESC_48              5096
#define PORT_ID_49                5097
#define PORT_DESC_49              5098
#define PORT_ID_50                5099
#define PORT_DESC_50              5100
#define PORT_ID_51                5101
#define PORT_DESC_51              5102
#define PORT_ID_52                5103
#define PORT_DESC_52              5104
#define PORT_ID_53                5105
#define PORT_DESC_53              5106
#define PORT_ID_54                5107
#define PORT_DESC_54              5108
#define PORT_ID_55                5109
#define PORT_DESC_55              5110
#define PORT_ID_56                5111
#define PORT_DESC_56              5112
#define PORT_ID_57                5113
#define PORT_DESC_57              5114
#define PORT_ID_58                5115
#define PORT_DESC_58              5116
#define PORT_ID_59                5117
#define PORT_DESC_59              5118
#define PORT_ID_60                5119
#define PORT_DESC_60              5120
#define PORT_ID_61                5121
#define PORT_DESC_61              5122
#define PORT_ID_62                5123
#define PORT_DESC_62              5124
#define PORT_ID_63                5125
#define PORT_DESC_63              5126
#define PORT_ID_64                5127
#define PORT_DESC_64              5128


#define PORT_HELP_TITLE           5900
#define PORT_ERR_TITLE            7000

#define PDR_ID_PROPERTIES         6001

#define PORT_ID_FIRST             PORT_ID_1
#define PORT_ID_LAST              PORT_ID_64
#define MAX_PORTS                 64

#define CUPS_ICON                7001

#define STR_LEN_PORTNAME          12
#define STR_LEN_PORTDESC          256
#define STR_LEN_DESC              81
#define PORT_ENTRY_LEN            256
#define STR_LEN_TITLE             256

#define STR_LEN_HOSTNAME          64
#define STR_LEN_QUEUENAME         64

   /*
   ** port defaults
   */
#define DEF_INITIALIZATION        "localhost#PRINT;"
#define DEF_TERMINATION           ";"
#define DEF_PORTDRIVER            "CUPS;"
#define DEF_PORTNAME              "CUPS"    /* Also set in build script - make sure they match! */
#define DEF_TIMEOUT               "45;"

#define APPNAME_LEAD_STR          "PM_"

#define KEY_DESCRIPTION           "DESCRIPTION"
#define KEY_INITIALIZATION        "INITIALIZATION"
#define KEY_TERMINATION           "TERMINATION"
#define KEY_PORTDRIVER            "PORTDRIVER"
#define KEY_TIMEOUT               "TIMEOUT"

#define APPNAME_PM_SPOOLER_PORT   "PM_SPOOLER_PORT"
#define IDD_CUPS                  200
#define ID_NUMBER_OF_DESC_LINES   8000
#define ID_FIRST_DESC_LINES       8001

// Used to pass data to port properties dialog procedure
typedef struct _LPRDATA
{
   HAB      hAB;
   HMODULE  hModule;
   PSZ      pszPortName;
   PSZ      pszAppName;
   CHAR     szSaveLprSetting[PORT_ENTRY_LEN+1];
   ULONG    lfModified;
} LPRDATA, *PLPRDATA;

// Port-specific data format used by PrtSet/SplPdSet
typedef struct _PORTSETTINGS {
   CHAR     szHost[ STR_LEN_HOSTNAME + 1 ];
   CHAR     szQueue[ STR_LEN_QUEUENAME + 1 ];
} PORTSETTINGS, *PPORTSETTINGS;

#define ID_IP                       201
#define ID_CUPSQUEUE                202
#define ID_BINARY                   203
/* #define ID_SPOOL_PATH               204 */

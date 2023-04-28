#include "usart.h"
#include "main.h"
#include "atk_mo1218.h"
#include "GPS.h"
#include "stdio.h"
#include "string.h"	 
#include "math.h"
void GPS_init()
{
		uint8_t ret=1;
		/* 初始化ATK-MO1218模块 */
	ret = atk_mo1218_init(38400);
	 if (ret != 0)
    {
  			printf("ATK-MO1218 init failed!\r\n");
        while (1);
    }
		/* 配置ATK-MO1218模块 */
    ret  = atk_mo1218_factory_reset(ATK_MO1218_FACTORY_RESET_REBOOT);
    ret += atk_mo1218_config_output_type(ATK_MO1218_OUTPUT_NMEA, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_nmea_msg(1, 1, 1, 1, 1, 1, 0, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_position_rate(ATK_MO1218_POSITION_RATE_5HZ, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_gnss_for_navigation(ATK_MO1218_GNSS_GPS_BEIDOU, ATK_MO1218_SAVE_SRAM_FLASH);
    if (ret != 0)
    {
        printf("ATK-MO1218 configure failed!\r\n");
        while (1);
    }
}






//以下是偏移到百度地图的算法
static double pi = 3.14159265358979324;      //圆周率
static double ee = 0.00669342162296594323;   //椭球的偏心率
static double a = 6378245.0;                 //卫星椭球坐标投影到平面地图坐标系的投影因子
static double x_pi = 3.14159265358979324 * 3000.0 / 180.0;  //圆周率转换量
 
 
// 求弧度
float radian(float d)
{
    return d * pi / 180.0;   //角度1? = π / 180
}
 
 
float transformLat(float lat, float lon)    //纬度转化
{
	float ret = -100.0f + 2.0f * lat + 3.0f * lon + 0.2f * lon * lon + 0.1f * lat * lon + 0.2f * sqrt(fabs(lat));
   ret += (20.0f * sin(6.0f * lat * pi) + 20.0f * sin(2.0f * lat * pi)) * 2.0f / 3.0f;
   ret += (20.0f * sin(lon * pi) + 40.0f * sin(lon / 3.0f * pi)) * 2.0f / 3.0f;
   ret += (160.0f * sin(lon / 12.0f * pi) + 320 * sin(lon * pi  / 30.0f)) * 2.0f / 3.0f;
   return ret;
}
 
 
float transformLon(float lat,float lon)   //经度转化
{
	 float ret = 300.0f + lat + 2.0f * lon + 0.1f * lat * lat + 0.1f * lat * lon + 0.1f * sqrt(fabs(lat));
   ret += (20.0f * sin(6.0f * lat * pi) + 20.0f * sin(2.0f * lat * pi)) * 2.0f / 3.0f;
   ret += (20.0f * sin(lat * pi) + 40.0f * sin(lat / 3.0f * pi)) * 2.0f / 3.0f;
   ret += (150.0f * sin(lat / 12.0f * pi) + 300.0f * sin(lat / 30.0f * pi)) * 2.0f / 3.0f;
   return ret;
}
 
 
/*****************************************************************************
 * WGS84(GPS坐标系) to 火星坐标系(GCJ-02)
 * 
 * @param lat
 * @param lon
 * @return
 ****************************************************************************/

void GPS84_To_GCJ02(float WGS84_Lat, float WGS84_Lon,float * GCJ02_Lat, float * GCJ02_Lon)
{
	float dLat;
	float dLon;
	float radLat;
	float magic;
	float sqrtMagic;
 
	dLat = transformLat(WGS84_Lon - 105.0f, WGS84_Lat - 35.0f);
	dLon = transformLon(WGS84_Lon - 105.0f, WGS84_Lat - 35.0f);
	radLat = WGS84_Lat / 180.0f * pi;
	magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0f) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
	dLon = (dLon * 180.0f) / (a / sqrtMagic * cos(radLat) * pi);
	*GCJ02_Lat = WGS84_Lat + dLat;    //GCJ02_Lat是百度纬度存储变量的地址  *GCJ02_Lat就是那个值
	*GCJ02_Lon = WGS84_Lon + dLon;    //GCJ02_Lon是百度经度存储变量的地址  *GCJ02_Lon
}
 
/*****************************************
 * 火星坐标系 (GCJ-02) 与百度坐标系 (BD-09) 的转换算法 将 GCJ-02 坐标转换成 BD-09 坐标
 * 
 * @param gg_lat
 * @param gg_lon
 *****************************************/
//传入的参数  	GCJ02_To_BD09(*BD09_Lat,*BD09_Lon,BD09_Lat,BD09_Lon);
//(*BD09_Lat,*BD09_Lon)火星坐标,(BD09_Lat,BD09_Lon)是变量的地址
void GCJ02_To_BD09(float GCJ02_Lat,float GCJ02_Lon,float * BD_09_Lat,float * BD_09_Lon)
{
		float x,y;
	  float z;
	  float theta;
		 	
	x = GCJ02_Lon;y = GCJ02_Lat;
		z= sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
		theta =atan2(y, x) + 0.000003 * cos(x * x_pi);
	  *BD_09_Lon = z * cos(theta) + 0.0065;
	  *BD_09_Lat = z * sin(theta) + 0.006;
}
 
 
/*************************************************************
函数名称：GPS_transformation(double WGS84_Lat, double WGS84_Lon,double * BD_09_Lat, double * BD_09_Lon)
函数功能：GPS坐标转百度地图坐标
输入参数：WGS84_Lat,WGS84_Lon GPS获取到真实经纬度  储存得到的百度经纬度变量的地址 BD_09_Lat,BD_09_Lon指向那个变量
输出参数：
*************************************************************/ 
void GPS_transformation(float WGS84_Lat,float WGS84_Lon,float * BD_09_Lat,float * BD_09_Lon)
{
	
	 GPS84_To_GCJ02(WGS84_Lat,WGS84_Lon,BD_09_Lat,BD_09_Lon);           //GPS坐标转火星坐标

	 GCJ02_To_BD09(*BD_09_Lat,*BD_09_Lon,BD_09_Lat,BD_09_Lon);           //火星坐标转百度坐标 
}






 /* 读取经纬度（放大了100000倍数） */


 uint8_t GPS_update(atk_mo1218_position_t position)
{
	float *tpla;	//纬度
 	float *tpln;	//经度
	uint8_t ret;
  atk_mo1218_time_t utc;
  
  int16_t altitude;
  uint16_t speed;
  atk_mo1218_fix_info_t fix_info;
//  atk_mo1218_visible_satellite_info_t gps_satellite_info = {0};
//  atk_mo1218_visible_satellite_info_t beidou_satellite_info = {0};
//  uint8_t satellite_index;

	tpla=(float*)malloc(6 *sizeof(tpla));
	tpln=(float*)malloc(6 *sizeof(tpln));
	
	ret = atk_mo1218_update(&utc, &position, &altitude, &speed, &fix_info, NULL, NULL, 5000);
	if (ret == ATK_MO1218_EOK)
      {
				*tpla= position.longitude.degree /100000;
				*tpln=position.latitude.degree / 100000;
		printf("Position: %d.%d'%s %d.%d'%s\r\n", position.longitude.degree / 100000, position.longitude.degree % 100000, (position.longitude.indicator == ATK_MO1218_LONGITUDE_EAST) ? "E" : "W",
				position.latitude.degree / 100000, position.latitude.degree % 100000, (position.latitude.indicator == ATK_MO1218_LATITUDE_NORTH) ? "N" : "S");
			GPS_transformation(*tpla,*tpln,tpla,tpln);
			}
			else
      { 
          /* ATK-MO1218模块未定位时，
           * 不输出NMEA协议的GSV语句，
           * 导致因获取不到可见GPS、北斗卫星的信息而超时失败，
           * 此时可将函数atk_mo1218_update()的入参gps_satellite_info和beidou_satellite_info
           * 传入NULL，从而获取未定位时的其他数据
           */
          printf("Error!\r\n");
      }
			free(tpla);free(tpln);
			return 0;
}





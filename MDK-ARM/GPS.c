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
		/* ��ʼ��ATK-MO1218ģ�� */
	ret = atk_mo1218_init(38400);
	 if (ret != 0)
    {
  			printf("ATK-MO1218 init failed!\r\n");
        while (1);
    }
		/* ����ATK-MO1218ģ�� */
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






//������ƫ�Ƶ��ٶȵ�ͼ���㷨
static double pi = 3.14159265358979324;      //Բ����
static double ee = 0.00669342162296594323;   //�����ƫ����
static double a = 6378245.0;                 //������������ͶӰ��ƽ���ͼ����ϵ��ͶӰ����
static double x_pi = 3.14159265358979324 * 3000.0 / 180.0;  //Բ����ת����
 
 
// �󻡶�
float radian(float d)
{
    return d * pi / 180.0;   //�Ƕ�1? = �� / 180
}
 
 
float transformLat(float lat, float lon)    //γ��ת��
{
	float ret = -100.0f + 2.0f * lat + 3.0f * lon + 0.2f * lon * lon + 0.1f * lat * lon + 0.2f * sqrt(fabs(lat));
   ret += (20.0f * sin(6.0f * lat * pi) + 20.0f * sin(2.0f * lat * pi)) * 2.0f / 3.0f;
   ret += (20.0f * sin(lon * pi) + 40.0f * sin(lon / 3.0f * pi)) * 2.0f / 3.0f;
   ret += (160.0f * sin(lon / 12.0f * pi) + 320 * sin(lon * pi  / 30.0f)) * 2.0f / 3.0f;
   return ret;
}
 
 
float transformLon(float lat,float lon)   //����ת��
{
	 float ret = 300.0f + lat + 2.0f * lon + 0.1f * lat * lat + 0.1f * lat * lon + 0.1f * sqrt(fabs(lat));
   ret += (20.0f * sin(6.0f * lat * pi) + 20.0f * sin(2.0f * lat * pi)) * 2.0f / 3.0f;
   ret += (20.0f * sin(lat * pi) + 40.0f * sin(lat / 3.0f * pi)) * 2.0f / 3.0f;
   ret += (150.0f * sin(lat / 12.0f * pi) + 300.0f * sin(lat / 30.0f * pi)) * 2.0f / 3.0f;
   return ret;
}
 
 
/*****************************************************************************
 * WGS84(GPS����ϵ) to ��������ϵ(GCJ-02)
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
	*GCJ02_Lat = WGS84_Lat + dLat;    //GCJ02_Lat�ǰٶ�γ�ȴ洢�����ĵ�ַ  *GCJ02_Lat�����Ǹ�ֵ
	*GCJ02_Lon = WGS84_Lon + dLon;    //GCJ02_Lon�ǰٶȾ��ȴ洢�����ĵ�ַ  *GCJ02_Lon
}
 
/*****************************************
 * ��������ϵ (GCJ-02) ��ٶ�����ϵ (BD-09) ��ת���㷨 �� GCJ-02 ����ת���� BD-09 ����
 * 
 * @param gg_lat
 * @param gg_lon
 *****************************************/
//����Ĳ���  	GCJ02_To_BD09(*BD09_Lat,*BD09_Lon,BD09_Lat,BD09_Lon);
//(*BD09_Lat,*BD09_Lon)��������,(BD09_Lat,BD09_Lon)�Ǳ����ĵ�ַ
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
�������ƣ�GPS_transformation(double WGS84_Lat, double WGS84_Lon,double * BD_09_Lat, double * BD_09_Lon)
�������ܣ�GPS����ת�ٶȵ�ͼ����
���������WGS84_Lat,WGS84_Lon GPS��ȡ����ʵ��γ��  ����õ��İٶȾ�γ�ȱ����ĵ�ַ BD_09_Lat,BD_09_Lonָ���Ǹ�����
���������
*************************************************************/ 
void GPS_transformation(float WGS84_Lat,float WGS84_Lon,float * BD_09_Lat,float * BD_09_Lon)
{
	
	 GPS84_To_GCJ02(WGS84_Lat,WGS84_Lon,BD_09_Lat,BD_09_Lon);           //GPS����ת��������

	 GCJ02_To_BD09(*BD_09_Lat,*BD_09_Lon,BD_09_Lat,BD_09_Lon);           //��������ת�ٶ����� 
}






 /* ��ȡ��γ�ȣ��Ŵ���100000������ */


 uint8_t GPS_update(atk_mo1218_position_t position)
{
	float *tpla;	//γ��
 	float *tpln;	//����
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
          /* ATK-MO1218ģ��δ��λʱ��
           * �����NMEAЭ���GSV��䣬
           * �������ȡ�����ɼ�GPS���������ǵ���Ϣ����ʱʧ�ܣ�
           * ��ʱ�ɽ�����atk_mo1218_update()�����gps_satellite_info��beidou_satellite_info
           * ����NULL���Ӷ���ȡδ��λʱ����������
           */
          printf("Error!\r\n");
      }
			free(tpla);free(tpln);
			return 0;
}





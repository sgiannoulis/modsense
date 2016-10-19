/*
 * gps_parser.h
 *
 * Created: 26/11/2013 4:14:14 μμ
 *  Author: spilios
 */ 


#ifndef GPS_PARSER_H_
#define GPS_PARSER_H_


typedef struct {
	bool gps_locked;
	unsigned char date[7];          // 0, 0, 0
	unsigned char time[7];          // 0, 0, 0
	char latitude[12];              // 3814.4648N
	char longitude[13];             // 02143.8936E
}gps_data_t;


bool init_gps(void);
uint8_t parse_gps_data(void);

gps_data_t* get_gps_data(void);

#endif /* GPS_PARSER_H_ */
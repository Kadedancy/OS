#pragma once

int printf(const char* fmt, ... ) __attribute__((format (printf , 1, 2 ) ));

int sprintf(char* s, const char* fmt, ... ) __attribute__((format (printf , 2, 3 ) ));

int snprintf(char* s, int bufferSize, const char* fmt, ... ) __attribute__((format (printf , 3, 4 ) ));

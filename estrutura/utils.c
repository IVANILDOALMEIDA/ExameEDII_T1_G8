#include "utils.h"
#include <time.h>
#include <stddef.h>
#include <ctype.h>

DataHora obter_data_hora_atual(void) {
    DataHora dh = {0, 0, 0, 0, 0};

    time_t agora = time(NULL);           
    struct tm *info = localtime(&agora);   /* converte para data/hora "legível" */

    if (info != NULL) {
        dh.dia    = info->tm_mday;          /* 1-31 */
        dh.mes    = info->tm_mon + 1;       /* tm_mon vai de 0-11 */
        dh.ano    = info->tm_year + 1900;   /* tm_year conta a partir de 1900 */
        dh.hora   = info->tm_hour;          /* 0-23 */
        dh.minuto = info->tm_min;           /* 0-59 */
    }

    return dh;
}

int comparar_data_hora(DataHora a, DataHora b) {
    if (a.ano    != b.ano)    return a.ano    - b.ano;
    if (a.mes    != b.mes)    return a.mes    - b.mes;
    if (a.dia    != b.dia)    return a.dia    - b.dia;
    if (a.hora   != b.hora)   return a.hora   - b.hora;
    if (a.minuto != b.minuto) return a.minuto - b.minuto;
    return 0;
}


int comparar_strings_ci(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char) *a);
        int cb = tolower((unsigned char) *b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return (unsigned char) *a - (unsigned char) *b;
}
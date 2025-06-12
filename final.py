import urequests, ujson, xtools, utime
from machine import UART, RTC

xtools.connect_wifi_led("A610","24622192")
rtc = RTC()

com = UART(2, 9600, tx=17, rx=16)
com.init(10600)

API_key = "8a5c22d99b3fd1f5a915bc4f49d2d339"
cities = ["Taipei","New Taipei","Keelung","Taoyuan","Hsinchu","Miaoli","Taichung",
          "Nantou","Yunlin","Chiayi","Tainan","Kaohsiung","Pingtung","Taitung",
          "Hualien","Yilan"]
city_index = 0
country = "TW"

def get_weather():
    url  = "https://api.openweathermap.org/data/2.5/weather?"
    url += "q=" + cities[city_index] + "," + country
    url += "&units=metric&lang=zh_tw&"
    url += "appid=" + API_key
    return url

def send_weather_to_uart(url):
    try:
        response = urequests.get(url)
        data1 = ujson.loads(response.text)
    except:
        print("取得天氣資料失敗")
        return

    try:
        main = data1["main"]
        temp = main["temp"]    
        humi = main["humidity"]  
        temp_str = "{:.2f}".format(temp).replace(".", "")  
        humi_str = "{:.2f}".format(humi).replace(".", "")  
        data_str = temp_str + humi_str   # 合併
        
    except:
        print("解析天氣資料失敗")
        return

    print("城市：", cities[city_index])
    print("溫度：", temp_str)
    print("濕度：", humi_str)
    for char in data_str:
        com.write(char.encode())
        utime.sleep(2)
        print("送出字元:", char)

while True:
    url = get_weather()
    send_weather_to_uart(url)
    city_index = (city_index + 1) % len(cities)
    utime.sleep(10)  # 每10秒輪一個城市

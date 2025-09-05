# Интеграция с китайскими регами протокол 808

JSON для регистрации - необходимо реализовать при регистрации рега:

```json
{
	"phoneNumber" : "191111781319",
	"provinceID" : 108,
	"cityID" : 23,
	"manufacturerID" : "13579",
	"terminalModel" : "QWERVDUIOPASDFGHJKLZ",
	"terminalID" : "QAZWSBV",
	"licensePlateColor" : 0,
	"VIN" : "WVGZZZCAZJC559100"
}
```


"phoneNumber" - номер телефона модема из 12 символов (цифр).


JSON сохранения статуса отправки события (ID сообщения + UID события + статус):

```json
{
  "jsonrpc": "2.0",
  "id": ..., // random int or string
  "method": "save",
  "params": {
    "auth": {"token": "..."}, // getenv("MTP_ES_TOKEN_LOCAL")
    "declaration": "808-alarm",
    "entity": {
      "d4b29894d6774e18a78a044d62b31f4c": {
        "timestamp": "2025-02-07 11:49:39",
        "event": "01K42VV78BPGKQTFH7A7KE29VA",
        "status": "pending"
      }
    }
  }
}
```


JSON актуализации статуса(ID сообщения + статус):

```json
{
  "jsonrpc": "2.0",
  "id": ..., // random int or string
  "method": "save",
  "params": {
    "auth": {"token": "..."}, // getenv("MTP_ES_TOKEN_LOCAL")
    "declaration": "808-alarm",
    "entity": {
      "d4b29894d6774e18a78a044d62b31f4c": {
        "status": "confirmed"
      }
    }
  }
}
```


JSON запрос UID по ID сообщения:

```json
{
  "jsonrpc": "2.0",
  "id": ..., // random int or string
  "method": "query",
  "params": {
    "auth": {"token": "..."}, // getenv("MTP_ES_TOKEN_LOCAL")
    "declaration": "808-alarm",
    "entity": ["d4b29894d6774e18a78a044d62b31f4c"]
  }
}
```

ответ:

```json
{
  "jsonrpc": "2.0",
  "id": ..., // = как в запросе
  "result": {
    "entity": {
      "d4b29894d6774e18a78a044d62b31f4c": {
        "timestamp": "2025-02-07 11:49:39",
        "event": "01K42VV78BPGKQTFH7A7KE29VA",
        "status": "confirmed"
      }
    }
  }
}
```

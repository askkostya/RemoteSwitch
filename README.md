<h1>Инструкция по установке RemoteSwitch</h1>
<h2>1. Начало работ. Введение</h2>
<blockquote>
<p>Данное устройство позволяет управлять (замыкать) любые кнопки с помощью программы Blynk, не теряя при этом функциональность аппаратных кнопок, ими по-прежнему можно пользоваться. Можно коммутировать цепи с постоянным током, но поскольку предназначалось в первую очередь для теплового шкафа LG &ndash; имеет определенную специфику.</p>
<p>Для работы используется контроллер ESP32 который подключается к сети WIFI. Для полноценного использования необходимо в том месте где вы подключаете RemoteSwitch иметь подключение к интернет. RS- выступает в качестве клиента WFI</p>
</blockquote>

<h2>2. Установка и настройка программы Blynk.</h2>

<h3>Установите на телефон или планшет программу</h3>

В GooglePlay - https://play.google.com/store/apps/details?id=cc.blynk&hl=ru

В AppStore - https://apps.apple.com/ru/app/blynk-iot-for-arduino-esp32/id808760481

![Image alt](https://github.com/askkostya/RemoteSwitch/raw/master/docs/RemoteSwitchDoc1.png)

Подробнее про установку приложения на телефон или планшет можно посмотреть тут
https://docs.blynk.cc/#downloads-blynk-apps-for-ios-or-android

<h3> Настройка интерфейса программы </h3>

<table style="height: 191px; width: 516px;">
<tbody>
<tr>
<td style="width: 80px;" rowspan="7"><img src="https://github.com/askkostya/RemoteSwitch/raw/master/docs/BlynkButton.jpg" /></td>
<td style="width: 80px; border-color: #36383c;">1</td>
<td style="width: 207px; border-color: #36383c;">
<p>Элемент - Gauge<br />Input-V4<br /><br /><br /></p>
</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">2</td>
<td style="border-color: #36383c;">Элемент Gauge<br />Input - V5<br />Refresh Rate - 30 sec</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">3</td>
<td style="border-color: #36383c;">Элемент - Styled Button<br />Output - V1<br />Mode - Switch</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">4</td>
<td style="border-color: #36383c;">Элемент - Styled Button<br />Output - V10<br />Mode - Switch</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">5</td>
<td style="border-color: #36383c;">
<p>Элемент - Styled Button<br />Output - V2<br />Mode - Switch</p>
</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">6</td>
<td style="border-color: #36383c;">Элемент - Styled Button<br />Output - V0<br />Mode - Switch</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
<tr>
<td style="border-color: #36383c;">7</td>
<td style="border-color: #36383c;">Элемент - Segment Switch<br />Output - V3<br />1 - Нагрев<br />2 - Вентилятор&nbsp;</td>
<td style="width: 32px;">&nbsp;</td>
</tr>
</tbody>
</table>

<h2>3. Настройка платы.</h2>

Перевести выключатель на плате в положение ON и включить устройство. Через некоторое время на телефоне должна появиться новая беспроводная сеть. Пароль по умолчанию 12345678

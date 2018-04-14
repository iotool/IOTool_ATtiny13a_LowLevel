# ATtiny13A Power Costumation

FUSE Calculator http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny13a

## Test CPU @ 5V

### 9.6 MHz

* FUSE: -U lfuse:w:0x6A:m -U hfuse:w:0xFF:m
* CPU: 1.2 MHz (9.6 MHz prescale 8)
* BOD: brown-out detection disabled
* ADC: on/off (analog ditigal converter)
* ACD: on/off (analog comparator)
* TIMER0: on/off

<table>
<tr>
  <td>CLKPR</td>
  <td>0</td>
  <td>1</td>
  <td>2</td>
  <td>3</td>
  <td>4</td>
  <td>5</td>
  <td>6</td>
  <td>7</td>
  <td>8</td>
</tr>
<tr>
  <td>DIV</td>
  <td>1</td>
  <td>2</td>
  <td>4</td>
  <td>8</td>
  <td>16</td>
  <td>32</td>
  <td>64</td>
  <td>128</td>
  <td>256</td>
</tr>
<tr>
  <td>CPU</td>
  <td>9,6MHz</td>
  <td>4,8MHz</td>
  <td>2,4MHz</td>
  <td>1,2MHz</td>
  <td>600kHz</td>
  <td>300kHz</td>
  <td>150kHz</td>
  <td>75kHz</td>
  <td>37,5kHz</td>
</tr>
<tr>
  <td>Prescale</td>
  <td>4,33mA</td><!--1//-->
  <td>2,85mA</td><!--2//-->
  <td>1,89mA</td><!--3//-->
  <td>1,30mA</td><!--4//-->
  <td>0,930mA</td><!--5//-->
  <td>0,707mA</td><!--6//-->
  <td>0,616mA</td><!--7//-->
  <td>0,551mA</td><!--8//-->
  <td>0,548mA</td><!--9//-->
</tr>
<tr>
  <td>ADC off</td>
  <td>4,27mA</td><!--1//-->
  <td>2,83mA</td><!--2//-->
  <td>1,88mA</td><!--3//-->
  <td>1,28mA</td><!--4//-->
  <td>0,850mA</td><!--5//-->
  <td>0,701mA</td><!--6//-->
  <td>0,592mA</td><!--7//-->
  <td>0,540mA</td><!--8//-->
  <td>0,520mA</td><!--9//-->
</tr>
<tr>
  <td>ADC* off</td>
  <td>4,17mA</td><!--1//-->
  <td>2,77mA</td><!--2//-->
  <td>1,84mA</td><!--3//-->
  <td>1,25mA</td><!--4//-->
  <td>0,860mA</td><!--5//-->
  <td>0,690mA</td><!--6//-->
  <td>0,590mA</td><!--7//-->
  <td>0,548mA</td><!--8//-->
  <td>0,531mA</td><!--9//-->
</tr>
<tr>
  <td>ACD off</td>
  <td>4,22mA</td><!--1//-->
  <td>2,76mA</td><!--2//-->
  <td>1,83mA</td><!--3//-->
  <td>1,164mA</td><!--4//-->
  <td>0,793mA</td><!--5//-->
  <td>0,632mA</td><!--6//-->
  <td>0,536mA</td><!--7//-->
  <td>0,492mA</td><!--8//-->
  <td>0,461mA</td><!--9//-->
</tr>
<tr>
  <td>TIM0 off</td>
  <td>4,06mA</td><!--1//-->
  <td>2,68mA</td><!--2//-->
  <td>1,77mA</td><!--3//-->
  <td>1,114mA</td><!--4//-->
  <td>0,776mA</td><!--5//-->
  <td>0,607</td><!--6//-->
  <td>0,511mA</td><!--7//-->
  <td>0,475mA</td><!--8//-->
  <td>0,431mA</td><!--9//-->
</tr>
</table>
* PRADC

### 128 kHz

* FUSE: -U lfuse:w:0x7B:m -U hfuse:w:0xFF:m
* CPU: 128 kHz (128 kHz prescale 1)
* BOD: brown-out detection disabled
* ADC: on/off (analog ditigal converter)
* ACD: on/off (analog comparator)

<table>
<tr>
  <td>CLKPR</td>
  <td>0</td>
  <td>1</td>
  <td>2</td>
  <td>3</td>
  <td>4</td>
  <td>5</td>
  <td>6</td>
  <td>7</td>
  <td>8</td>
</tr>
<tr>
  <td>DIV</td>
  <td>1</td>
  <td>2</td>
  <td>4</td>
  <td>8</td>
  <td>16</td>
  <td>32</td>
  <td>64</td>
  <td>128</td>
  <td>256</td>
</tr>
<tr>
  <td>CPU</td>
  <td>128kHz</td>
  <td>64kHz</td>
  <td>32kHz</td>
  <td>16kHz</td>
  <td>8kHz</td>
  <td>4kHz</td>
  <td>2kHz</td>
  <td>1kHz</td>
  <td>500Hz</td>
</tr>
<tr>
  <td>Prescale</td>
  <td>518uA</td><!--1//-->
  <td>489uA</td><!--2//-->
  <td>473uA</td><!--3//-->
  <td>448uA</td><!--4//-->
  <td>459uA</td><!--5//-->
  <td>460uA</td><!--6//-->
  <td>432uA</td><!--7//-->
  <td>440uA</td><!--8//-->
  <td>445uA</td><!--9//-->
</tr>
<tr>
  <td>ADC off</td>
  <td>512uA</td><!--1//-->
  <td>492uA</td><!--2//-->
  <td>475uA</td><!--3//-->
  <td>473uA</td><!--4//-->
  <td>451uA</td><!--5//-->
  <td>444uA</td><!--6//-->
  <td>438uA</td><!--7//-->
  <td>459uA</td><!--8//-->
  <td>445uA</td><!--9//-->
</tr>
<tr>
  <td>ACD off</td>
  <td>462uA</td><!--1//-->
  <td>433uA</td><!--2//-->
  <td>389uA</td><!--3//-->
  <td>379uA</td><!--4//-->
  <td>392uA</td><!--5//-->
  <td>371uA</td><!--6//-->
  <td>369uA</td><!--7//-->
  <td>364uA</td><!--8//-->
  <td>357uA</td><!--9//-->
</tr>
</table>

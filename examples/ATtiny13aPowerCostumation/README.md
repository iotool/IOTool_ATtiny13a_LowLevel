# ATtiny13A Power Costumation

FUSE Calculator http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny13a

## Test CPU @ 5V

* FUSE: -U lfuse:w:0x6A:m -U hfuse:w:0xFF:m
* CPU: 1.2 MHz (9.6 MHz prescale 8)

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
</table>

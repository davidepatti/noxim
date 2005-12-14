<?php
  require_once("shared/functions.php");  
  print_header();
?>

<p>Set up the configuration parameters for the simulation:</p>

<form name="data" action="noxim.php" method="POST">
<table border="1">
<tr><td>Mesh X Dimension</td><td><select name="dimx">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><td>Mesh Y Dimension</td><td><select name="dimy">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><td>Buffer Depth</td><td><select name="buffer">
<?php
  for($i=1; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i flits (Default)</option>\n");
    else print("<option value='$i'>$i flits</option>\n");
  }
?>
</select></td></tr>
<tr><td>Maximum Packet Size</td><td><select name="size">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==10) print("<option value='$i' selected>$i flits (Default)</option>\n");
    else print("<option value='$i'>$i flits</option>\n");
  }
?>
</select></td></tr>
<tr><td>Routing Algorithm</td><td><select name="routing">
  <option value="xy" selected>X-Y (Default)</option>
  <option value="westfirst">West First</option>
  <option value="northlast">North Last</option>
  <option value="negativefirst">Negative First</option>
  <option value="oddeven">Odd-Even</option>
  <option value="dyad">Dyad</option>
  <option value="lookahead">Look-Ahead</option>
  <option value="nopcar">Nopcar</option>
  <option value="fullyadaptive">Fully Adaptive</option>
</select></td></tr>
<tr><td>Selection Strategy</td><td><select name="sel">
  <option value="random" selected>Random (Default)</option>
  <option value="bufferlevel">Buffer-level</option>
  <option value="nopcar">Nopcar</option>
</select></td></tr>
<tr><td>Packet Injection Rate</td><td><select name="pir">
<?php
  for($i=0.01; $i<=1; $i+=0.1) {
    if($i==0.01) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><td>Simulation Time</td><td><select name="run">
  <option value="5000">5 &micro;s</option>
  <option value="10000" selected>10 &micro;s (Default)</option>
  <option value="20000">20 &micro;s</option>
  <option value="30000">30 &micro;s</option>
  <option value="40000">40 &micro;s</option>
  <option value="50000">50 &micro;s</option>
  <option value="75000">75 &micro;s</option>
  <option value="100000">100 &micro;s</option>
</select></td></tr>
<tr><td colspan="2"><input type="submit" value="Run the simulation &gt;&gt;"></td></tr>
</table>
</form>

<?php
  print_footer();
?>
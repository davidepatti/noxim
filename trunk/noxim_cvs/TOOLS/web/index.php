<?php
  require_once("shared/functions.php");  
  print_header();
?>

<p>Set up the configuration parameters for the simulation:</p>

<form name="data" action="noxim.php" method="POST">
<table border="1">
<tr><th>Mesh X Dimension</th><td><select name="dimx">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><th>Mesh Y Dimension</th><td><select name="dimy">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><th>Buffer Depth</th><td><select name="buffer">
<?php
  for($i=1; $i<=20; $i++) {
    if($i==4) print("<option value='$i' selected>$i flits (Default)</option>\n");
    else print("<option value='$i'>$i flits</option>\n");
  }
?>
</select></td></tr>
<tr><th>Maximum Packet Size</th><td><select name="size">
<?php
  for($i=2; $i<=20; $i++) {
    if($i==10) print("<option value='$i' selected>$i flits (Default)</option>\n");
    else print("<option value='$i'>$i flits</option>\n");
  }
?>
</select></td></tr>
<tr><th>Routing Algorithm</th><td><select name="routing">
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
<tr><th>Selection Strategy</th><td><select name="sel">
  <option value="random" selected>Random (Default)</option>
  <option value="bufferlevel">Buffer-level</option>
  <option value="nopcar">Nopcar</option>
</select></td></tr>
<tr><th>Packet Injection Rate</th><td><select name="pir">
<?php
  for($i=0.01; $i<=1; $i+=0.1) {
    if($i==0.01) print("<option value='$i' selected>$i (Default)</option>\n");
    else print("<option value='$i'>$i</option>\n");
  }
?>
</select></td></tr>
<tr><th>Traffic Distribution</th><td><select name="traffic">
  <option value="uniform" selected>Uniform (Default)</option>
  <option value="transpose1">Transpose 1</option>
  <option value="transpose2">Transpose 2</option>
</select></td></tr>
<tr><th>Simulation Time</th><td><select name="sim">
  <option value="5000">5'000 cycles</option>
  <option value="10000" selected>10'000 cycles (Default)</option>
  <option value="20000">20'000 cycles</option>
  <option value="30000">30'000 cycles</option>
  <option value="40000">40'000 cycles</option>
  <option value="50000">50'000 cycles</option>
  <option value="75000">75'000 cycles</option>
  <option value="100000">100'000 cycles</option>
</select></td></tr>
<tr><th colspan="2"><input type="submit" value="Run the simulation &gt;&gt;"></th></tr>
</table>
</form>

<?php
  print_footer();
?>
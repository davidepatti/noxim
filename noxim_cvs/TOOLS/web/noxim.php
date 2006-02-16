<?php
  require_once("shared/functions.php");  
  print_header();

  $command = "/usr/local/bin/noxim";
  if($_POST["dimx"]>1 && $_POST["dimx"]<=20) $command.=(" -dimx ".$_POST["dimx"]);
  if($_POST["dimy"]>1 && $_POST["dimy"]<=20) $command.=(" -dimy ".$_POST["dimy"]);
  if($_POST["buffer"]>=1 && $_POST["buffer"]<=20) $command.=(" -buffer ".$_POST["buffer"]);
  if($_POST["size_min"]>1 && $_POST["size_min"]<=20 && $_POST["size_max"]>1 && $_POST["size_max"]<=20 && $_POST["size_min"]<=$_POST["size_max"]) $command.=(" -size ".$_POST["size_min"]." ".$_POST["size_max"]);
  if(in_array($_POST["routing"], array("xy","westfirst","northlast","negativefirst","oddeven","dyad","fullyadaptive"))) $command.=(" -routing ".$_POST["routing"]);
  if(in_array($_POST["sel"], array("random","bufferlevel","nop"))) $command.=(" -sel ".$_POST["sel"]);
  if($_POST["pir"]>0 && $_POST["pir"]<=1) $command.=(" -pir ".$_POST["pir"]);
  if(in_array($_POST["traffic"], array("random","transpose1","transpose2"))) $command.=(" -traffic ".$_POST["traffic"]);
  if($_POST["sim"]>1000 && $_POST["sim"]<=100000) $command.=(" -sim ".$_POST["sim"]);

  $output = shell_exec($command." 2>&1");
  $all_values = explode(" ", $output);
  $delay = floatval($all_values[count($all_values)-6]);
  if($delay>0) {
    print("<p>Executed command <tt>$command</tt> and obtained:</p>\n");
    print("<table border='1'><tr><th>Global average delay</th><td>$delay</td></tr></table>\n");
  } else {
    print("<p>Command <tt>$command</tt> exited with errors:</p>\n");
    print("<pre>$output</pre>\n");
  }  

  print("<p><input type='button' value='New simulation &lt;&lt;' onClick='javascript:history.go(-1);'></p>");

  print_footer();
?>
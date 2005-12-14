<?php
  require_once("shared/functions.php");  
  print_header();

  $command = "noxim";
  if($_POST["dimx"]>1 && $_POST["dimx"]<=20) $command.=(" -dimx ".$_POST["dimx"]);
  if($_POST["dimy"]>1 && $_POST["dimy"]<=20) $command.=(" -dimy ".$_POST["dimy"]);
  if($_POST["buffer"]>=1 && $_POST["buffer"]<=20) $command.=(" -buffer ".$_POST["buffer"]);
  if($_POST["size"]>1 && $_POST["size"]<=20) $command.=(" -size ".$_POST["size"]);
  if(in_array($_POST["routing"], array("xy","westfirst","northlast","negativefirst","oddeven","dyad","lookahead","nopcar","fullyadaptive"))) $command.=(" -routing ".$_POST["routing"]);
  if(in_array($_POST["sel"], array("random","bufferlevel","nopcar"))) $command.=(" -sel ".$_POST["sel"]);
  if($_POST["pir"]>0 && $_POST["pir"]<=1) $command.=(" -pir ".$_POST["pir"]);
  if($_POST["run"]>1000 && $_POST["run"]<=100000) $command.=(" -run ".$_POST["run"]);

  $output = shell_exec($command." 2>&1");
  $all_values = explode(" ", $output);
  $delay = $all_values[count($all_values)-1];
  if($delay>0) {
    print("<p>Executed command <tt>$command</tt> and obtained:</p>\n");
    print("<table border='1'><tr><td>Global average delay</td><th>$delay</th></tr></table>\n");
  } else {
    print("<p>Command <tt>$command</tt> exited with errors:</p>\n");
    print("<pre>$output</pre>\n");
  }

  print("<p><input type='button' value='New simulation &lt;&lt;' onClick='javascript:history.go(-1);'></p>");

  print_footer();
?>
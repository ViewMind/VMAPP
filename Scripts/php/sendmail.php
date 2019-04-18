<?php  
  require_once 'Swift/swift_required.php';  

  // ================================= USING GMAIL SMTP ===============================  
  $transport = Swift_SmtpTransport::newInstance('smtp.gmail.com', 465,'ssl');
  $transport->setUsername('check.viewmind');
  $transport->setPassword('ViMi1987');
  // ===================================================================================  

  // Create the Mailer using your created Transport
  $mailer = Swift_Mailer::newInstance($transport);
  
  $subject = "Test Email";
  $from    = "test@mail.com";
  $to      = "aarelovich@gmail.com";
  $body    = "This is a test message";
  
  $message = Swift_Message::newInstance($subject);
  $message->setFrom($from);
  $message->setTo($to);
  $message->setBody($body, 'text/html');
  $message->attach(Swift_Attachment::fromPath('test.jpg')->setFilename('MazinKaizerHaou.jpg'));
  
  try{
    $result = $mailer->send($message,$failures);
  }
  catch (Swift_TransportException $e){
    echo 'Hubo un error al enviar los mails: ' . $e->getMessage() .  "\n";
  }
  
  if (count($failures) > 0){
     $mail["failures"] = implode(", ",$failures);
  }
  
  ?>

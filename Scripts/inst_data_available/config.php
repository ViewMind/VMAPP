<?php
     $config = array ( "sync_info" => [
                                        array("institution_uid"  => "989302458",
                                              "institution_user" => "novartis",
                                              "IP"               => "68.183.67.134") 
                                      ],
                                
                  "dbconfig" =>  array ('db_data_host' => 'viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com',
                                        'db_data_name' => 'viewmind_data',
                                        'db_data_user' => 'vmuser',
                                        'db_data_passwd' => 'viewmind_2018',
                                        'db_patid_host' => 'viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com',
                                        'db_patid_name' => 'viewmind_id',
                                        'db_patid_user' => 'vmid',
                                        'db_patid_passwd' => 'ksjdn3228shs1852'),
                  
                  "ext_leave_on_cleanup" => ["csv", "dat", "datf", "rep"],
                  
                  "report_variable_names" => array('reading_predicted_deterioration' => 'INDEX OF COGNITIVE IMPAIRMENT',
                                                   'executive_proceseses' => 'EXECUTIVE PROCESSES',
                                                   'working_memory' => 'WORKING MEMORY',
                                                   'retrieval_memory' => 'RETRIEVAL MEMORY',
                                                   'binding_conversion_index' => 'INDEX OF AMYLOID BETA PROTEIN CORRELATION',
                                                   'behavioural_response' => 'BEHAVIOURAL RESPONSE',
                                                   'number_of_fixations_enc' => 'ENCODING EFFICIENCY',
                                                   'number_of_fixations_ret' => 'DECODING EFFICIENCY',
                                                   'inhibitory_problems_percent' => 'INHIBITORY PROCESSES',
                                                   'correct_complete_sequence_percent' => 'SEQUENCE COMPLETION',
                                                   'correct_target_hit_percent' => 'TARGET HIT',
                                                   'mean_response_time' => 'MEAN RESPONSE TIME',
                                                   'mean_saccade_amplitude' => 'VISUAL EXPLORATION EFFICIENCY'),
                                  
                  "run_local" => false
                 ); 
?>

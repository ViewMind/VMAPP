ACCESS = {};
ACCESS.COMMON = {}
ACCESS.COMMON.KEY = "keyid";

ACCESS.SUBJECT = {}
ACCESS.SUBJECT.NAME                 = "name";
ACCESS.SUBJECT.LAST_NAME            = "last_name";
ACCESS.SUBJECT.BIRTH_COUNTRY_CODE   = "birth_country_code";
ACCESS.SUBJECT.BIRTH_DATE           = "birth_date";
ACCESS.SUBJECT.AGE                  = "age";
ACCESS.SUBJECT.YEARS_FORMATION      = "years_formation";
ACCESS.SUBJECT.INTERNAL_ID          = "internal_id";
ACCESS.SUBJECT.INSTITUTION_ID       = "institution_id";
ACCESS.SUBJECT.INSTANCE_NUMBER      = "instance_number";
ACCESS.SUBJECT.UNIQUE_ID            = "unique_id";
ACCESS.SUBJECT.LASTEST              = "lastest";
ACCESS.SUBJECT.GENDER               = "gender";

ACCESS.EVALUATIONS = {}
ACCESS.EVALUATIONS.PORTAL_USER          =  "portal_user";
ACCESS.EVALUATIONS.STUDY_TYPE           =  "study_type";
ACCESS.EVALUATIONS.STUDY_DATE           =  "study_date";
ACCESS.EVALUATIONS.PROCESSING_DATE      =  "processing_date";
ACCESS.EVALUATIONS.INSTITUTION_ID       =  "institution_id";
ACCESS.EVALUATIONS.INSTITUTION_INSTANCE =  "institution_instance";
ACCESS.EVALUATIONS.SUBJECT_ID           =  "subject_id";
ACCESS.EVALUATIONS.EVALUATOR_EMAIL      =  "evaluator_email";
ACCESS.EVALUATIONS.EVALUATOR_NAME       =  "evaluator_name";
ACCESS.EVALUATIONS.EVALUATOR_LASTNAME   =  "evaluator_lastname";
ACCESS.EVALUATIONS.PROTOCOL             =  "protocol";
ACCESS.EVALUATIONS.RESULTS              =  "results";
ACCESS.EVALUATIONS.DISCARD_REASON       =  "discard_reason";
ACCESS.EVALUATIONS.QC_PARAMS            =  "qc_parameters";
ACCESS.EVALUATIONS.STUDY_CONFIG         =  "study_configuration";
ACCESS.EVALUATIONS.QC_GRAPHS            =  "qc_graphs";

ACCESS.PORTALUSER = {}
ACCESS.PORTALUSER.NAME                  = "name";
ACCESS.PORTALUSER.LASTNAME              = "lastname";
ACCESS.PORTALUSER.EMAIL                 = "email";
ACCESS.PORTALUSER.PASSWD                = "passwd";
ACCESS.PORTALUSER.COMPANY               = "company";

// Study Result Segment Key Values
ACCESS.RESSEG = {}
ACCESS.RESSEG.TITLE          = "title_"
ACCESS.RESSEG.EXPLANATION    = "explanation_"
ACCESS.RESSEG.RANGE_TEXT     = "range_text_"
ACCESS.RESSEG.CUTOFFVALS     = "cutoff_values"
ACCESS.RESSEG.SMALLER_BETTER = "smaller_better"
ACCESS.RESSEG.RES_KEY        = "associated_value"
ACCESS.RESSEG.MULT100        = "mult_by_100"
ACCESS.RESSEG.ROUND_FOR_DISP = "round_for_display"

// General Report Key Values
ACCESS.REPTEXT = {};
ACCESS.REPTEXT.MOTTO         = "motto"
ACCESS.REPTEXT.RESULTS       = "results"
ACCESS.REPTEXT.COLORINTRO    = "color_into"
ACCESS.REPTEXT.BLUE          = "index_blue"
ACCESS.REPTEXT.GREEN         = "index_green"
ACCESS.REPTEXT.YELLOW        = "index_yellow"
ACCESS.REPTEXT.RED           = "index_red"
ACCESS.REPTEXT.PATIENT       = "patient"
ACCESS.REPTEXT.AGE           = "age"
ACCESS.REPTEXT.DATE          = "date"
ACCESS.REPTEXT.REQDR         = "reqdr"

// Checking if the report contains discard reason and comment. 
ACCESS.REPORT = {};
ACCESS.REPORT.DISCARD_REASON = "discard_reason";
ACCESS.REPORT.COMMENT        = "comment";
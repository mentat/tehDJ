#-clisp (require :loop)

(defun translate (&optional old new)
  ;; if not old, translate every file in current directory
  ;; if old but not new, translate and overwrite old
  (if old
      (if new
	  (translate-file old new)
	(let ((trouble (translate-file old "temporary-translation")))
	  (when (not trouble)
	    (rename-file old (concatenate 'string old ".orig"))
	    (rename-file "temporary-translation" old))))
    (let ((dir (directory ".")))
      (loop for fil in dir do
	(if (or (string= (pathname-type fil) "c")
		(string= (pathname-type fil) "h")
		(string= (pathname-type fil) "html")
		(string= (pathname-type fil) "def")
		)
	    (let ((file (namestring fil)))
	      (let ((trouble (translate-file file "temporary-translation")))
		(format t "~A..." file) (force-output)
		(if (not trouble)
		    (progn
		      (rename-file file (concatenate 'string file ".orig"))
		      (rename-file "temporary-translation" file)
		      (format t "ok~%"))
		  (format t "oops~%")))))))))

(defvar transtable
  (list
   ;; (list old new)
   (list "SNDLIB_LITTLE_ENDIAN" "MUS_LITTLE_ENDIAN")
   (list "SNDLIB_SAMPLE_TYPE" "MUS_SAMPLE_TYPE")
   (list "SNDLIB_SAMPLE_BITS" "MUS_SAMPLE_BITS")
   (list "SNDLIB_SAMPLE_0" "MUS_SAMPLE_0")
   (list "SNDLIB_BYTE_TO_SAMPLE" "MUS_BYTE_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_BYTE" "MUS_SAMPLE_TO_BYTE")
   (list "SNDLIB_SHORT_TO_SAMPLE" "MUS_SHORT_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_SHORT" "MUS_SAMPLE_TO_SHORT")
   (list "SNDLIB_INT24_TO_SAMPLE" "MUS_INT24_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_INT24" "MUS_SAMPLE_TO_INT24")
   (list "SNDLIB_INT_TO_SAMPLE" "MUS_INT_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_INT" "MUS_SAMPLE_TO_INT")
   (list "SNDLIB_SNDFIX" "MUS_FLOAT_TO_FIX")
   (list "SNDLIB_SNDFLT" "MUS_FIX_TO_FLOAT")
   (list "SNDLIB_FLOAT_TO_SAMPLE" "MUS_FLOAT_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_FLOAT" "MUS_SAMPLE_TO_FLOAT")
   (list "SNDLIB_DOUBLE_TO_SAMPLE" "MUS_DOUBLE_TO_SAMPLE")
   (list "SNDLIB_SAMPLE_TO_DOUBLE" "MUS_SAMPLE_TO_DOUBLE")
   (list "SNDLIB_SAMPLE_MAX" "MUS_SAMPLE_MAX")
   (list "SNDLIB_SAMPLE_MIN" "MUS_SAMPLE_MIN")
   (list "SNDLIB_MIX_MAX" "MUS_MIX_MAX")
   (list "SNDLIB_MIX_MIN" "MUS_MIX_MIN")
   (list "SNDLIB_DAC_CHANNEL" "MUS_DAC_CHANNEL")
   (list "SNDLIB_DAC_REVERB" "MUS_DAC_REVERB")
   (list "SNDLIB_HDR_UNSUPPORTED" "MUS_UNSUPPORTED")
   (list "SNDLIB_HDR_NEXT" "MUS_NEXT")
   (list "SNDLIB_HDR_AIFC" "MUS_AIFC")
   (list "SNDLIB_HDR_RIFF" "MUS_RIFF")
   (list "SNDLIB_HDR_BICSF" "MUS_BICSF")
   (list "SNDLIB_HDR_NIST" "MUS_NIST")
   (list "SNDLIB_HDR_INRS" "MUS_INRS")
   (list "SNDLIB_HDR_ESPS" "MUS_ESPS")
   (list "SNDLIB_HDR_SVX" "MUS_SVX")
   (list "SNDLIB_HDR_VOC" "MUS_VOC")
   (list "SNDLIB_HDR_SNDT" "MUS_SNDT")
   (list "SNDLIB_HDR_RAW" "MUS_RAW")
   (list "SNDLIB_HDR_SMP" "MUS_SMP")
   (list "SNDLIB_HDR_SD2" "MUS_SD2")
   (list "SNDLIB_HDR_AVR" "MUS_AVR")
   (list "SNDLIB_HDR_IRCAM" "MUS_IRCAM")
   (list "SNDLIB_HDR_SD1" "MUS_SD1")
   (list "SNDLIB_HDR_SPPACK" "MUS_SPPACK")
   (list "SNDLIB_HDR_MUS10" "MUS_MUS10")
   (list "SNDLIB_HDR_HCOM" "MUS_HCOM")
   (list "SNDLIB_HDR_PSION" "MUS_PSION")
   (list "SNDLIB_HDR_MAUD" "MUS_MAUD")
   (list "SNDLIB_HDR_IEEE" "MUS_IEEE")
   (list "SNDLIB_HDR_DESKMATE" "MUS_DESKMATE")
   (list "SNDLIB_HDR_DESKMATE_2500" "MUS_DESKMATE_2500")
   (list "SNDLIB_HDR_MATLAB" "MUS_MATLAB")
   (list "SNDLIB_HDR_ADC" "MUS_ADC")
   (list "SNDLIB_HDR_SOUND_EDIT" "MUS_SOUND_EDIT")
   (list "SNDLIB_HDR_SOUND_EDIT_16" "MUS_SOUND_EDIT_16")
   (list "SNDLIB_HDR_DVSM" "MUS_DVSM")
   (list "SNDLIB_HDR_MIDI" "MUS_MIDI")
   (list "SNDLIB_HDR_ESIGNAL" "MUS_ESIGNAL")
   (list "SNDLIB_HDR_SOUNDFONT" "MUS_SOUNDFONT")
   (list "SNDLIB_HDR_GRAVIS" "MUS_GRAVIS")
   (list "SNDLIB_HDR_COMDISCO" "MUS_COMDISCO")
   (list "SNDLIB_HDR_GOLDWAVE" "MUS_GOLDWAVE")
   (list "SNDLIB_HDR_SRFS" "MUS_SRFS")
   (list "SNDLIB_HDR_MIDI_SAMPLE_DUMP" "MUS_MIDI_SAMPLE_DUMP")
   (list "SNDLIB_HDR_DIAMONDWARE" "MUS_DIAMONDWARE")
   (list "SNDLIB_HDR_REALAUDIO" "MUS_REALAUDIO")
   (list "SNDLIB_HDR_ADF" "MUS_ADF")
   (list "SNDLIB_HDR_SBSTUDIOII" "MUS_SBSTUDIOII")
   (list "SNDLIB_HDR_DELUSION" "MUS_DELUSION")
   (list "SNDLIB_HDR_FARANDOLE" "MUS_FARANDOLE")
   (list "SNDLIB_HDR_SAMPLE_DUMP" "MUS_SAMPLE_DUMP")
   (list "SNDLIB_HDR_ULTRATRACKER" "MUS_ULTRATRACKER")
   (list "SNDLIB_HDR_YAMAHA_SY85" "MUS_YAMAHA_SY85")
   (list "SNDLIB_HDR_YAMAHA_TX16" "MUS_YAMAHA_TX16")
   (list "SNDLIB_HDR_DIGIPLAYER" "MUS_DIGIPLAYER")
   (list "SNDLIB_HDR_COVOX" "MUS_COVOX")
   (list "SNDLIB_HDR_SPL" "MUS_SPL")
   (list "SNDLIB_HDR_AVI" "MUS_AVI")
   (list "SNDLIB_HDR_OMF" "MUS_OMF")
   (list "SNDLIB_HDR_QUICKTIME" "MUS_QUICKTIME")
   (list "SNDLIB_HDR_ASF" "MUS_ASF")
   (list "SNDLIB_HDR_YAMAHA_SY99" "MUS_YAMAHA_SY99")
   (list "SNDLIB_HDR_KURZWEIL_2000" "MUS_KURZWEIL_2000")
   (list "SNDLIB_HDR_AIFF" "MUS_AIFF")
   (list "SNDLIB_HDR_PAF" "MUS_PAF")
   (list "SNDLIB_HEADER_TYPE_OK" "MUS_HEADER_TYPE_OK")
   (list "SNDLIB_UNSUPPORTED" "MUS_UNSUPPORTED")
   (list "SNDLIB_NO_SND" "MUS_UNKNOWN")
   (list "SNDLIB_16_LINEAR" "MUS_BSHORT")
   (list "SNDLIB_8_MULAW" "MUS_MULAW")
   (list "SNDLIB_8_LINEAR" "MUS_BYTE")
   (list "SNDLIB_32_FLOAT" "MUS_BFLOAT")
   (list "SNDLIB_32_LINEAR" "MUS_BINT")
   (list "SNDLIB_8_ALAW" "MUS_ALAW")
   (list "SNDLIB_8_UNSIGNED" "MUS_UBYTE")
   (list "SNDLIB_24_LINEAR" "MUS_B24INT")
   (list "SNDLIB_64_DOUBLE" "MUS_BDOUBLE")
   (list "SNDLIB_16_LINEAR_LITTLE_ENDIAN" "MUS_LSHORT")
   (list "SNDLIB_32_LINEAR_LITTLE_ENDIAN" "MUS_LINT")
   (list "SNDLIB_32_FLOAT_LITTLE_ENDIAN" "MUS_LFLOAT")
   (list "SNDLIB_64_DOUBLE_LITTLE_ENDIAN" "MUS_LDOUBLE")
   (list "SNDLIB_16_UNSIGNED" "MUS_UBSHORT")
   (list "SNDLIB_16_UNSIGNED_LITTLE_ENDIAN" "MUS_ULSHORT")
   (list "SNDLIB_24_LINEAR_LITTLE_ENDIAN" "MUS_L24INT")
   (list "SNDLIB_32_VAX_FLOAT" "MUS_VAXFLOAT")
   (list "SNDLIB_12_LINEAR" "MUS_B12INT")
   (list "SNDLIB_12_LINEAR_LITTLE_ENDIAN" "MUS_L12INT")
   (list "SNDLIB_12_UNSIGNED" "MUS_UB12INT")
   (list "SNDLIB_12_UNSIGNED_LITTLE_ENDIAN" "MUS_UL12INT")
   (list "SNDLIB_DATA_FORMAT_OK" "MUS_DATA_FORMAT_OK")
   (list "SNDLIB_COMPATIBLE_FORMAT" "MUS_COMPATIBLE_FORMAT")
   (list "SNDLIB_OUT_FORMAT" "MUS_OUT_FORMAT")
   (list "SNDLIB_NIST_shortpack" "MUS_NIST_SHORTPACK")
   (list "SNDLIB_AIFF_IMA_ADPCM" "MUS_AIFF_IMA_ADPCM")
   (list "SNDLIB_DEFAULT_DEVICE" "MUS_AUDIO_DEFAULT")
   (list "SNDLIB_READ_WRITE_DEVICE" "MUS_AUDIO_DUPLEX_DEFAULT")
   (list "SNDLIB_ADAT_IN_DEVICE" "MUS_AUDIO_ADAT_IN")
   (list "SNDLIB_AES_IN_DEVICE" "MUS_AUDIO_AES_IN")
   (list "SNDLIB_LINE_OUT_DEVICE" "MUS_AUDIO_LINE_OUT")
   (list "SNDLIB_LINE_IN_DEVICE" "MUS_AUDIO_LINE_IN")
   (list "SNDLIB_MICROPHONE_DEVICE" "MUS_AUDIO_MICROPHONE")
   (list "SNDLIB_SPEAKERS_DEVICE" "MUS_AUDIO_SPEAKERS")
   (list "SNDLIB_DIGITAL_IN_DEVICE" "MUS_AUDIO_DIGITAL_IN")
   (list "SNDLIB_DIGITAL_OUT_DEVICE" "MUS_AUDIO_DIGITAL_OUT")
   (list "SNDLIB_DAC_OUT_DEVICE" "MUS_AUDIO_DAC_OUT")
   (list "SNDLIB_ADAT_OUT_DEVICE" "MUS_AUDIO_ADAT_OUT")
   (list "SNDLIB_AES_OUT_DEVICE" "MUS_AUDIO_AES_OUT")
   (list "SNDLIB_DAC_FILTER_DEVICE" "MUS_AUDIO_DAC_FILTER")
   (list "SNDLIB_MIXER_DEVICE" "MUS_AUDIO_MIXER")
   (list "SNDLIB_LINE1_DEVICE" "MUS_AUDIO_LINE1")
   (list "SNDLIB_LINE2_DEVICE" "MUS_AUDIO_LINE2")
   (list "SNDLIB_LINE3_DEVICE" "MUS_AUDIO_LINE3")
   (list "SNDLIB_AUX_INPUT_DEVICE" "MUS_AUDIO_AUX_INPUT")
   (list "SNDLIB_CD_IN_DEVICE" "MUS_AUDIO_CD")
   (list "SNDLIB_AUX_OUTPUT_DEVICE" "MUS_AUDIO_AUX_OUTPUT")
   (list "SNDLIB_SPDIF_IN_DEVICE" "MUS_AUDIO_SPDIF_IN")
   (list "SNDLIB_SPDIF_OUT_DEVICE" "MUS_AUDIO_SPDIF_OUT")
   (list "SNDLIB_AUDIO_SYSTEM" "MUS_AUDIO_PACK_SYSTEM")
   (list "SNDLIB_SYSTEM" "MUS_AUDIO_SYSTEM")
   (list "SNDLIB_DEVICE" "MUS_AUDIO_DEVICE")
   (list "SNDLIB_NO_ERROR" "MUS_AUDIO_NO_ERROR")
   (list "SNDLIB_CHANNELS_NOT_AVAILABLE" "MUS_AUDIO_CHANNELS_NOT_AVAILABLE")
   (list "SNDLIB_SRATE_NOT_AVAILABLE" "MUS_AUDIO_SRATE_NOT_AVAILABLE")
   (list "SNDLIB_FORMAT_NOT_AVAILABLE" "MUS_AUDIO_FORMAT_NOT_AVAILABLE")
   (list "SNDLIB_NO_INPUT_AVAILABLE" "MUS_AUDIO_NO_INPUT_AVAILABLE")
   (list "SNDLIB_NO_OUTPUT_AVAILABLE" "MUS_AUDIO_NO_OUTPUT_AVAILABLE")
   (list "SNDLIB_INPUT_BUSY" "MUS_AUDIO_INPUT_BUSY")
   (list "SNDLIB_OUTPUT_BUSY" "MUS_AUDIO_OUTPUT_BUSY")
   (list "SNDLIB_CONFIGURATION_NOT_AVAILABLE" "MUS_AUDIO_CONFIGURATION_NOT_AVAILABLE")
   (list "SNDLIB_INPUT_CLOSED" "MUS_AUDIO_INPUT_CLOSED")
   (list "SNDLIB_OUTPUT_CLOSED" "MUS_AUDIO_OUTPUT_CLOSED")
   (list "SNDLIB_IO_INTERRUPTED" "MUS_AUDIO_IO_INTERRUPTED")
   (list "SNDLIB_NO_LINES_AVAILABLE" "MUS_AUDIO_NO_LINES_AVAILABLE")
   (list "SNDLIB_WRITE_ERROR" "MUS_AUDIO_WRITE_ERROR")
   (list "SNDLIB_SIZE_NOT_AVAILABLE" "MUS_AUDIO_SIZE_NOT_AVAILABLE")
   (list "SNDLIB_DEVICE_NOT_AVAILABLE" "MUS_AUDIO_DEVICE_NOT_AVAILABLE")
   (list "SNDLIB_CANT_CLOSE" "MUS_AUDIO_CANT_CLOSE")
   (list "SNDLIB_CANT_OPEN" "MUS_AUDIO_CANT_OPEN")
   (list "SNDLIB_READ_ERROR" "MUS_AUDIO_READ_ERROR")
   (list "SNDLIB_AMP_NOT_AVAILABLE" "MUS_AUDIO_AMP_NOT_AVAILABLE")
   (list "SNDLIB_AUDIO_NO_OP" "MUS_AUDIO_NO_OP")
   (list "SNDLIB_CANT_WRITE" "MUS_AUDIO_CANT_WRITE")
   (list "SNDLIB_CANT_READ" "MUS_AUDIO_CANT_READ")
   (list "SNDLIB_NO_READ_PERMISSION" "MUS_AUDIO_NO_READ_PERMISSION")
   (list "SNDLIB_AMP_FIELD" "MUS_AUDIO_AMP")
   (list "SNDLIB_SRATE_FIELD" "MUS_AUDIO_SRATE")
   (list "SNDLIB_CHANNEL_FIELD" "MUS_AUDIO_CHANNEL")
   (list "SNDLIB_FORMAT_FIELD" "MUS_AUDIO_FORMAT")
   (list "SNDLIB_DEVICE_FIELD" "MUS_AUDIO_PORT")
   (list "SNDLIB_IMIX_FIELD" "MUS_AUDIO_IMIX")
   (list "SNDLIB_IGAIN_FIELD" "MUS_AUDIO_IGAIN")
   (list "SNDLIB_RECLEV_FIELD" "MUS_AUDIO_RECLEV")
   (list "SNDLIB_PCM_FIELD" "MUS_AUDIO_PCM")
   (list "SNDLIB_PCM2_FIELD" "MUS_AUDIO_PCM2")
   (list "SNDLIB_OGAIN_FIELD" "MUS_AUDIO_OGAIN")
   (list "SNDLIB_LINE_FIELD" "MUS_AUDIO_LINE")
   (list "SNDLIB_MIC_FIELD" "MUS_AUDIO_MICROPHONE")
   (list "SNDLIB_LINE1_FIELD" "MUS_AUDIO_LINE1")
   (list "SNDLIB_LINE2_FIELD" "MUS_AUDIO_LINE2")
   (list "SNDLIB_LINE3_FIELD" "MUS_AUDIO_LINE3")
   (list "SNDLIB_SYNTH_FIELD" "MUS_AUDIO_SYNTH")
   (list "SNDLIB_BASS_FIELD" "MUS_AUDIO_BASS")
   (list "SNDLIB_TREBLE_FIELD" "MUS_AUDIO_TREBLE")
   (list "SNDLIB_CD_FIELD" "MUS_AUDIO_CD")

   (list "SNDLIB_SAMPLE_ARRAY" "MUS_SAMPLE_ARRAY")
   (list "SNDLIB_MAKE_SAMPLE_ARRAY" "MUS_MAKE_SAMPLE_ARRAY")
   (list "SNDLIB_FREE_SAMPLE_ARRAY" "MUS_FREE_SAMPLE_ARRAY")

   (list "mus_set_error_handler" "mus_error_set_handler")
   (list "mus_make_error_tag" "mus_error_make_tag")
   (list "sound_samples" "mus_sound_samples")
   (list "sound_frames" "mus_sound_frames")
   (list "sound_datum_size" "mus_sound_datum_size")
   (list "sound_data_location" "mus_sound_data_location")
   (list "sound_chans" "mus_sound_chans")
   (list "sound_srate" "mus_sound_srate")
   (list "sound_header_type" "mus_sound_header_type")
   (list "sound_data_format" "mus_sound_data_format")
   (list "sound_original_format" "mus_sound_original_format")
   (list "sound_comment_start" "mus_sound_comment_start")
   (list "sound_comment_end" "mus_sound_comment_end")
   (list "sound_length" "mus_sound_length")
   (list "sound_fact_samples" "mus_sound_fact_samples")
   (list "sound_distributed" "mus_sound_distributed")
   (list "sound_write_date" "mus_sound_write_date")
   (list "sound_type_specifier" "mus_sound_type_specifier")
   (list "sound_align" "mus_sound_align")
   (list "sound_bits_per_sample" "mus_sound_bits_per_sample")
   (list "sound_type_name" "mus_header_type_name")
   (list "sound_format_name" "mus_data_format_name")
   (list "sound_comment" "mus_sound_comment")
   (list "sound_bytes_per_sample" "mus_data_format_to_bytes_per_sample")
   (list "sound_duration" "mus_sound_duration")
   (list "initialize_sndlib" "mus_sound_initialize")
   (list "finalize_sndlib" "mus_sound_finalize")
   (list "sndlib_sample_bits" "mus_sample_bits")
   (list "override_sound_header" "mus_sound_override_header")
   (list "forget_sound" "mus_sound_forget")
   (list "sound_print_cache" "mus_sound_print_cache")
   (list "sound_aiff_p" "mus_sound_aiff_p")
   (list "sound_loop_info" "mus_sound_loop_info")
   (list "sound_set_loop_info" "mus_sound_set_loop_info")
   (list "open_sound_input" "mus_sound_open_input")
   (list "open_sound_output" "mus_sound_open_output")
   (list "reopen_sound_output" "mus_sound_reopen_output")
   (list "close_sound_input" "mus_sound_close_input")
   (list "close_sound_output" "mus_sound_close_output")
   (list "read_sound" "mus_sound_read")
   (list "write_sound" "mus_sound_write")
   (list "seek_sound" "mus_sound_seek")
   (list "seek_sound_frame" "mus_sound_seek_frame")
   (list "sound_max_amp" "mus_sound_max_amp")
   (list "mus_file2array" "mus_file_to_array")
   (list "mus_array2file" "mus_array_to_file")
   (list "describe_audio_state" "mus_audio_describe")
   (list "report_audio_state" "mus_audio_report")
   (list "open_audio_output" "mus_audio_open_output")
   (list "open_audio_input" "mus_audio_open_input")
   (list "write_audio" "mus_audio_write")
   (list "close_audio" "mus_audio_close")
   (list "read_audio" "mus_audio_read")
   (list "read_audio_state" "mus_audio_mixer_read")
   (list "write_audio_state" "mus_audio_mixer_write")
   (list "save_audio_state" "mus_audio_save")
   (list "restore_audio_state" "mus_audio_restore")
   (list "audio_error" "mus_audio_error")
   (list "initialize_audio" "mus_audio_initialize")
   (list "audio_error_name" "mus_audio_error_name")
   (list "set_audio_error" "mus_audio_set_error")
   (list "audio_systems" "mus_audio_systems")
   (list "audio_system_name" "mus_audio_system_name")
   (list "audio_moniker" "mus_audio_moniker")
   (list "set_dsp_devices" "mus_audio_set_dsp_devices")
   (list "dsp_devices" "mus_audio_dsp_devices")
   (list "set_oss_buffers" "mus_audio_set_oss_buffers")
   (list "write_mixer_state" "mus_audio_mixer_save")
   (list "read_mixer_state" "mus_audio_mixer_restore")
   (list "sun_outputs" "mus_audio_sun_outputs")
   (list "clear_soundcard_inputs" "mus_audio_clear_soundcard_inputs")
   (list "mus_open_file_descriptors" "mus_file_open_descriptors")
   (list "mus_set_file_descriptors" "mus_file_set_descriptors")
   (list "mus_close_file_descriptors" "mus_file_close_descriptors")
   (list "mus_cleanup_file_descriptors" "mus_file_cleanup_descriptors")
   (list "mus_open_read" "mus_file_open_read")
   (list "mus_probe_file" "mus_file_probe")
   (list "mus_open_write" "mus_file_open_write")
   (list "mus_create" "mus_file_create")
   (list "mus_reopen_write" "mus_file_reopen_write")
   (list "mus_close" "mus_file_close")
   (list "mus_seek" "mus_file_seek")
   (list "mus_seek_frame" "mus_file_seek_frame")
   (list "mus_read" "mus_file_read")
   (list "mus_read_chans" "mus_file_read_chans")
   (list "mus_write_zeros" "mus_file_write_zeros")
   (list "mus_write" "mus_file_write")
   (list "mus_create_descriptors" "mus_file_create_descriptors")
   (list "mus_read_any" "mus_file_read_any")
   (list "mus_read_from_file" "mus_file_read_file")
   (list "mus_read_from_buffer" "mus_file_read_buffer")
   (list "mus_write_to_file" "mus_file_write_file")
   (list "mus_write_to_buffer" "mus_file_write_buffer")
   (list "mus_complete_filename" "mus_file_full_name")
   (list "mus_set_data_clipped" "mus_file_set_data_clipped")
   (list "mus_set_header_type" "mus_file_set_header_type")
   (list "mus_get_header_type" "mus_file_header_type")
   (list "mus_fd2file_name" "mus_file_fd_name")
   (list "mus_set_chans" "mus_file_set_chans")
   (list "mus_prescaler" "mus_file_prescaler")
   (list "mus_set_prescaler" "mus_file_set_prescaler")
   (list "mus_set_raw_header_defaults" "mus_header_set_raw_defaults")
   (list "mus_true_file_length" "mus_header_true_length")

   (list "mus_format2bytes" "mus_data_format_to_bytes_per_sample")
   (list "mus_header_format2bytes" "mus_header_data_format_to_bytes_per_sample")
   (list "mus_samples2bytes" "mus_samples_to_bytes")
   (list "mus_bytes2samples" "mus_bytes_to_samples")

   (list "mus_write_next_header" "mus_header_write_next_header")
   (list "mus_read_header_with_fd" "mus_header_read_with_fd")
   (list "mus_read_header" "mus_header_read")
   (list "mus_write_header" "mus_header_write")
   (list "mus_write_header_with_fd" "mus_header_write_with_fd")
   (list "mus_update_header_with_fd" "mus_header_update_with_fd")
   (list "mus_update_header" "mus_header_update")
   (list "mus_update_header_comment" "mus_header_update_comment")
   (list "mus_create_header_buffer" "mus_header_initialize")
   (list "mus_set_snd_header" "mus_header_snd_set_header")
   (list "mus_set_big_endian_int" "mus_bint_to_char")
   (list "mus_big_endian_int" "mus_char_to_bint")
   (list "mus_set_little_endian_int" "mus_lint_to_char")
   (list "mus_little_endian_int" "mus_char_to_lint")
   (list "mus_uninterpreted_int" "mus_char_to_uninterpreted_int")
   (list "mus_set_big_endian_float" "mus_bfloat_to_char")
   (list "mus_big_endian_float" "mus_char_to_bfloat")
   (list "mus_set_little_endian_float" "mus_lfloat_to_char")
   (list "mus_little_endian_float" "mus_char_to_lfloat")
   (list "mus_set_big_endian_short" "mus_bshort_to_char")
   (list "mus_big_endian_short" "mus_char_to_bshort")
   (list "mus_set_little_endian_short" "mus_lshort_to_char")
   (list "mus_little_endian_short" "mus_char_to_lshort")
   (list "mus_set_big_endian_unsigned_short" "mus_ubshort_to_char")
   (list "mus_big_endian_unsigned_short" "mus_char_to_ubshort")
   (list "mus_set_little_endian_unsigned_short" "mus_ulshort_to_char")
   (list "mus_little_endian_unsigned_short" "mus_char_to_ulshort")
   (list "mus_little_endian_double" "mus_char_to_ldouble")
   (list "mus_big_endian_double" "mus_char_to_bdouble")
   (list "mus_set_big_endian_double" "mus_bdouble_to_char")
   (list "mus_set_little_endian_double" "mus_ldouble_to_char")
   (list "mus_big_endian_unsigned_int" "mus_char_to_ubint")
   (list "mus_little_endian_unsigned_int" "mus_char_to_ulint")
   (list "mus_set_aifc_header" "mus_header_set_aifc")
   (list "init_sndlib2scm" "mus_sndlib2scm_initialize")
   (list "mus_header_type2string" "mus_header_type_name")
   (list "mus_header_data_format2string" "mus_data_format_name")
   (list "sndlib_initialize" "mus_sound_initialize")

   ))

(defun translate-file (old-file new-file)
  (with-open-file (ifile old-file)
    (with-open-file (ofile new-file :direction :output :if-exists :supersede :if-does-not-exist :create)
      (let ((cur-char (read-char ifile nil nil)) ;no error at eof but return nil
	    (cur-word nil))
	(loop while cur-char do
	  (if (and (not (alphanumericp cur-char))
		   (not (char= cur-char #\_)))
	      (progn
		(if cur-word
		    (let ((tolist (find cur-word transtable :test #'string= :key #'first)))
		      (if tolist (format t "replace: ~A ~A~%" cur-word (second tolist)))
		      (if tolist
			  (write-string (second tolist) ofile)
			(write-string cur-word ofile))
		      (setf cur-word nil)))
		(write-char cur-char ofile))
	    (if cur-word
		(setf cur-word (concatenate 'string cur-word (string cur-char)))
	      (setf cur-word (string cur-char))))
	  (setf cur-char (read-char ifile nil nil)))))))

   
   
   
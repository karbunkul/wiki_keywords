<?php

class wk{
  private $str;
  private $jwplayer = array();
  
  
  function __construct($text) {
    $this->str = $text;
    $this->jwplayer = array(
      'flag' => FALSE,
      'path' => variable_get('wiki_keywords_jwplayer_path'),
      'count' => 0,
      'container' => variable_get('wiki_keywords_jwplayer_container_name', 'media'),
      'id' => 0,
    );   
  }
  
  // Генерация вставки видео
  private function video($param) {
    $str = '';
    $playlist = array();
    
    $hostname = 'http://' . $_SERVER['HTTP_HOST'] . '/';
    
    if ($hostname == 'http://localhost/'){
      $hostname .= 'tgasu/';          
    }
    
    
    $settings = array(
      'width' => variable_get('wiki_keywords_jwplayer_width', 640),
      'height' => variable_get('wiki_keywords_jwplayer_height', 480),
      'playlist.position' => variable_get('wiki_keywords_jwplayer_playlist_position', t('none')),
      'flashplayer' =>$hostname . $this->jwplayer['path'] . '/player.swf',
      'skin' =>$hostname . $this->jwplayer['path'] . '/skins/beelden.zip',
      
    ); 
    
    // Если работаем с плейлистом
    if (isset($param->playlist)) {
      foreach ($param->playlist as $key => $val) {
        // Ссылка на видео
        if (is_string($val)) {
          $str = '{"file":"' . $val . '"}';
        }
        // Подробности плейлиста такие как title, image, description etc
        else if (is_object($val)) {
		  $str = json_encode($val);
	    }
	    $playlist[] = $str;
      }
    }
    
    // Если задано лишь одно видео
    if (isset($param->src) && (is_string($param->src))) {
	  
	  /* file exists remote addr
	  $image = '';
	  $fileinfo = pathinfo($param->src);
	  $img = $fileinfo['dirname'] . '/' . $fileinfo['filename'] . '.jpg';
	 
	    if (file_exists($img)){ dsm($img);
        $image = ', "image":"' . $fileinfo['dirname'] . '/' . $fileinfo['filename'] . '.jpg"';    
	    }
	  
	  $playlist[] = '{"file":"' . $param->src . '"' . $image .'}';
	  */
	  $playlist[] = '{"file":"' . $param->src . '"}';
	  
    }
    
    //
    if (isset($param->settings)) {
	  	
    }
    // Если в плейлисте 1 файл то скрываем плейлист
    if (sizeof($playlist) < 2) {
	  $settings['playlist.position'] = 'none';  	
    }
    
    $setup = '';
    // Формируем настройки
    foreach ($settings as $key => $val) {
	  	$setup .= sprintf('%s"%s":"%s", ', "\n", $key, $val);
    }

    $this->jwplayer['id'] = md5(time() . rand(0,65536));
    $str = sprintf('<center><div class="%s" id="id-%s">Loading the player ...</div></center>', $this->jwplayer['container'], $this->jwplayer['id']);
    $str .= sprintf('<script> jwplayer("id-%s").setup({%s playlist:[%s]});</script>', $this->jwplayer['id'], $setup, implode(",\n", $playlist));
	
    return $str;
    
  }
  
  public function replace() {
  //					
    $matches = array();
    $replaces = array();
  
    preg_match_all('/\[wk[\s\S]*?\/wk\]/', $this->str, $matches);
    $matches = $matches[0]; 
    
    for ($i = 0; $i < sizeof($matches); $i++) {
      $match = array();
      preg_match('/(?<=wk\]).*(?=\[\/wk\])/', $matches[$i], $match);
      $json = json_decode('{' . implode('', $match) . '}');
      
      //dsm($json);
      
      $replace = '';
      switch ($json->mode) {
        case 'video' : 
          $this->jwplayer['count']++;
          $replace = $this->video($json); 
		break;
      }
      
      if ($replace != '') {
        $replaces[$i] = $replace; 
      }
    }
  
    $str = $this->str;
    for ($i = 0; $i < sizeof($replaces); $i++) {
      $str = str_replace($matches[$i], $replaces[$i], $str);
    }
  
    return $str;
  }
  
}

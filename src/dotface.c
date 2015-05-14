#include <pebble.h>
#include <math.h> 
#include <string.h> 
Window *my_window;
InverterLayer *ilayer;
Layer *layer;

static char hour_buffer[10];
static char min_buffer[3];
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  layer_mark_dirty(window_get_root_layer(my_window));
}

static void graphics_draw_line2(GContext *ctx, GPoint p0, GPoint p1, int8_t width) {
  // Order points so that lower x is first
  int16_t x0, x1, y0, y1;
  if (p0.x <= p1.x) {
    x0 = p0.x; x1 = p1.x; y0 = p0.y; y1 = p1.y;
  } else {
    x0 = p1.x; x1 = p0.x; y0 = p1.y; y1 = p0.y;
  }
  
  // Init loop variables
  int16_t dx = x1-x0;
  int16_t dy = abs(y1-y0);
  int16_t sy = y0<y1 ? 1 : -1; 
  int16_t err = (dx>dy ? dx : -dy)/2;
  int16_t e2;
  
  // Calculate whether line thickness will be added vertically or horizontally based on line angle
  int8_t xdiff, ydiff;
  
  if (dx > dy) {
    xdiff = 0;
    ydiff = width/2;
  } else {
    xdiff = width/2;
    ydiff = 0;
  }
  
  // Use Bresenham's integer algorithm, with slight modification for line width, to draw line at any angle
  while (true) {
    // Draw line thickness at each point by drawing another line 
    // (horizontally when > +/-45 degrees, vertically when <= +/-45 degrees)
    graphics_draw_line(ctx, GPoint(x0-xdiff, y0-ydiff), GPoint(x0+xdiff, y0+ydiff));
    
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0++; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

static void my_layer_draw(Layer *layeer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int hr = t->tm_hour; 
  // Draw a white filled circle a radius of half the layer height
  graphics_context_set_fill_color(ctx, GColorBlack);
  //0,1,2,3 are the valid hours.
  //
  int xmult=1;
  int ymult=1;

  if(hr==12){hr=0;}

  if(hr>3 && hr <= 6){
    hr=hr-3;
    ymult=-1;
  }else if(hr > 6 && hr <=9){
    hr=hr-6;
    xmult=-1;
    ymult=-1;
  }else if(hr > 9){
    hr=hr-9;
    xmult=-1;
  }

  //float hr=0;
  if(xmult==1 && ymult==1){
    hr=3-hr;
  }
  //based on origin at 0,0
  double y=abs(sin(hr*22.5f)*60.0f);
  double x=abs(cos(hr*22.5f)*60.0f);
  y *= ymult;
  x *= xmult;
  //shift origin
  y= 84.0f-y;
  x=72+x;
  

int32_t hourHandLength=60;
int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_hour / 12;
y = (-cos_lookup(hour_angle) * hourHandLength / TRIG_MAX_RATIO) + 84;
x = (sin_lookup(hour_angle) * hourHandLength / TRIG_MAX_RATIO) + 72;

  graphics_fill_circle(ctx, GPoint((int)x, (int)y), 5);
int32_t secondHandLength=60;
int32_t second_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + 84;
x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + 72;
  
  graphics_draw_line2(ctx,GPoint(72,84), GPoint((int)x, (int)y),5);
  graphics_fill_circle(ctx, GPoint((int)x, (int)y), 2);
    graphics_fill_circle(ctx, GPoint((int)72, (int)84), 2);
//   graphics_draw_circle(ctx,GPoint(72,84),60);
}

void handle_init(void) {
  my_window = window_create();
  layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(layer, my_layer_draw);
  layer_add_child(window_get_root_layer(my_window), layer);
  ilayer = inverter_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(layer,inverter_layer_get_layer(ilayer));
  window_stack_push(my_window, true);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

}

void handle_deinit(void) {

  window_destroy(my_window);

}

int main(void) {
  hour_buffer[0]='1';
  hour_buffer[1]='0';
  hour_buffer[2]='\0';
  min_buffer[0]='0';
  min_buffer[1]='\0';
  handle_init();
  app_event_loop();
  handle_deinit();
}

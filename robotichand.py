import facom

class RoboticHand:
    'For controllling robotic hand'

    def __init__(self):
        'Initialize object'

        self.is_automatic = False
        self.is_rotate_up = False
        self.is_extend = False
        self.is_grab = False
        self.plc = facom.Facom()
        self.plc.open('/dev/ttyUSB0')
        self.init_state()

    def __del__(self):
        ' Cleanup object'
        pass

    def init_state(self):
        'Initialize robotic hand state'

        self.grab(False)
        self.move_up()
        self.move_left()
        self.extend(False)
        self.rotate_down()

    def automatic(self, automatic):
        'Activate/Deactivate automatic mode'
        if automatic:
            self.plc.set_discrete(facom.DISCRETE_M, 4, facom.ACTION_SET)
        else:
            self.plc.set_discrete(facom.DISCRETE_M, 4, facom.ACTION_RESET)
        self.is_automatic = automatic

    def move_up(self):
        'Move construction up'
        self.plc.set_discrete(facom.DISCRETE_M, 40, facom.ACTION_SET)

    def move_down(self):
        'Move construction down'
        self.plc.set_discrete(facom.DISCRETE_M, 41, facom.ACTION_SET)

    def move_right(self):
        'Move right'
        self.plc.set_discrete(facom.DISCRETE_M, 42, facom.ACTION_SET)

    def move_left(self):
        'Move Left'
        self.plc.set_discrete(facom.DISCRETE_M, 43, facom.ACTION_SET)

    def rotate_up(self):
        'Rotate Up'
        self.plc.set_discrete(facom.DISCRETE_M, 44, facom.ACTION_SET)
        self.is_rotate_up = True

    def rotate_down(self):
        'Rotate Down'
        self.plc.set_discrete(facom.DISCRETE_M, 45, facom.ACTION_SET)
        self.is_rotate_up = False

    def extend(self, extend):
        'Extend/UnExtend object'
        if extend:
            self.plc.set_discrete(facom.DISCRETE_M, 46, facom.ACTION_SET)
        else:
            self.plc.set_discrete(facom.DISCRETE_M, 47, facom.ACTION_SET)
    
    def grab(self, grab):
        'Pick/Place object'
        if grab:
            self.plc.set_discrete(facom.DISCRETE_M, 48, facom.ACTION_SET)
        else:
            self.plc.set_discrete(facom.DISCRETE_M, 49, facom.ACTION_SET)
        self.is_grab = grab

    def states(self):
        'Get robotic hand states'

        automatic = 'Automatic'
        rotation = 'Rotate Up'
        extend = 'Extend'
        grab = 'Pick'

        if self.is_automatic:
            automatic = 'Manual'
        if self.is_rotate_up:
            rotation = 'Rotate Down'
        if self.is_extend:
            extend = 'Un Extend'
        if self.is_grab:
            grab = 'Place'

        return {
            'automatic': automatic,
            'rotation': rotation,
            'extend': extend,
            'grab': grab
        }


import {Layout, makeScene2D} from '@motion-canvas/2d';
import {createRef} from '@motion-canvas/core';
import { AnimatedArray } from '../components/AnimatedArray';

const time = 0.5;

export default makeScene2D(function* (view) {
  const array = createRef<AnimatedArray>();

  view.add(
    <AnimatedArray ref={array} values={[1, 2, 3, 4, 5]} />
  );

  yield* array().changeColor(0, 'red', time);
  yield* array().changeColor(1, 'green', time);
  yield* array().push(6, time);
  yield* array().pop(time);
  yield* array().push_front(0, time);
  yield* array().pop_front(time);
  yield* array().push_front(0, time);
});

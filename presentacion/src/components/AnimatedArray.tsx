import {
  Layout,
  Node,
  NodeProps,
  Rect,
  Txt,
  initial,
  signal,
} from '@motion-canvas/2d';
import {
  Color,
  createRef,
  easeInOutCubic,
  Reference,
  SignalValue,
  SimpleSignal,
  tween,
} from '@motion-canvas/core';

export interface ArrayProps extends NodeProps {
  values: SignalValue<number[]>;
}

const bgColor = '#FFAC1C';
const highlightColor = '#FFD93D';

export class AnimatedArray extends Node {
  @initial([])
  @signal()
  public declare readonly values: SimpleSignal<number[], this>;

  private elements: Reference<Rect>[] = [];
  private colors: string[] = [];
  private layoutRef = createRef<Layout>();

  public constructor(props?: ArrayProps) {
    super({
      ...props,
    });

    this.elements = new Array(this.values().length).fill('').map(() => createRef<Rect>());
    this.colors = new Array(this.values().length).fill(bgColor);

    this.add(
      <Layout ref={this.layoutRef} direction="row" gap={12} layout>
        {this.values().map((_, index) => this.getRect(index))}
      </Layout>
    );
  }

  private getRect(index: number): Node {
    return <Rect ref={this.elements[index]} key={index.toString()} width={100} height={100} fill={this.colors[index]} justifyContent="center" alignItems="center" radius={12} stroke={this.colors[index]}>
    <Txt text={this.values()[index].toString()} />
  </Rect>
  }

  public *changeColor(index: number, color: string, seconds = 1) {    
    yield* tween(seconds, value => {
      this.elements[index]().fill(
        Color.lerp(
          new Color(this.colors[index]),
          new Color(color),
          easeInOutCubic(value),
        ),
      );
    });
    this.colors[index] = color;
  }

  public *push(value: number, seconds = 0.6) {
    const arr = [...this.values()];
    arr.push(value);
    this.values(arr);
    this.elements.push(createRef<Rect>());
    this.colors.push(bgColor);

    this.layoutRef().add(this.getRect(arr.length - 1));

    const idx = arr.length - 1;
    this.elements[idx]().scale(0);
    yield* tween(seconds, t => {
      this.elements[idx]().scale(easeInOutCubic(t));
    });
  }

  public *pop(seconds = 0.6): Generator<any, number | undefined, any> {
    if (this.values().length === 0) return undefined;
    const idx = this.values().length - 1;

    yield* tween(seconds, t => {
      this.elements[idx]().scale(1 - easeInOutCubic(t));
    });
    this.elements[idx]().remove();

    const arr = [...this.values()];
    const value = arr.pop();
    this.values(arr);

    this.elements.pop();
    this.colors.pop();

    return value;
  }

  public *push_front(value: number, seconds = 0.6) {
    const arr = [...this.values()];
    arr.unshift(value);
    this.values(arr);
    this.elements.unshift(createRef<Rect>());
    this.colors.unshift(bgColor);

    this.layoutRef().insert(this.getRect(0), 0);
    this.elements[0]().scale(0);
    yield* tween(seconds, t => {
      this.elements[0]().scale(easeInOutCubic(t));
    });
  }

  public *pop_front(seconds = 0.6): Generator<any, number | undefined, any> {
    if (this.values().length === 0) return undefined;

    yield* tween(seconds, t => {
      this.elements[0]().scale(1 - easeInOutCubic(t));
    });
    this.elements[0]().remove();

    const arr = [...this.values()];
    const value = arr.shift();
    this.values(arr);

    this.elements.shift();
    this.colors.shift();

    return value;
  }
}
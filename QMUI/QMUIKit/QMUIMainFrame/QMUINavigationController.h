//
//  QMUINavigationController.h
//  qmui
//
//  Created by QMUI Team on 14-6-24.
//  Copyright (c) 2014年 QMUI Team. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface QMUINavigationController : UINavigationController

/**
 *  初始化时调用的方法，会在 initWithNibName:bundle: 和 initWithCoder: 这两个指定的初始化方法中被调用，所以子类如果需要同时支持两个初始化方法，则建议把初始化时要做的事情放到这个方法里。否则仅需重写要支持的那个初始化方法即可。
 */
- (void)didInitialize NS_REQUIRES_SUPER;

@end

@interface QMUINavigationController (UISubclassingHooks)

/**
 *  每个界面Controller在即将展示的时候被调用，在`UINavigationController`的方法`navigationController:willShowViewController:animated:`中会自动被调用，同时因为如果把一个界面dismiss后回来此时并不会调用`navigationController:willShowViewController`，所以需要在`viewWillAppear`里面也会调用一次。
 */
- (void)willShowViewController:(nonnull UIViewController *)viewController animated:(BOOL)animated NS_REQUIRES_SUPER;

/**
 *  同上
 */
- (void)didShowViewController:(nonnull UIViewController *)viewController animated:(BOOL)animated NS_REQUIRES_SUPER;

@end


@interface UIViewController (QMUINavigationController)

/// 判断当前 viewController 是否处于手势返回中，仅对当前手势返回涉及到的前后两个 viewController 有效
@property(nonatomic, assign, readonly) BOOL qmui_navigationControllerPoppingInteracted;

/// 基本与上一个属性 qmui_navigationControllerPoppingInteracted 相同，只不过 qmui_navigationControllerPoppingInteracted 是在 began 时就为 YES，而这个属性仅在 changed 时才为 YES。
/// @note viewController 会在走完 viewWillAppear: 之后才将这个值置为 YES。
@property(nonatomic, assign, readonly) BOOL qmui_navigationControllerPopGestureRecognizerChanging;

/// 当前 viewController 是否正在被手势返回 pop
@property(nonatomic, assign, readonly) BOOL qmui_poppingByInteractivePopGestureRecognizer;

/// 当前 viewController 是否是手势返回中，背后的那个界面
@property(nonatomic, assign, readonly) BOOL qmui_willAppearByInteractivePopGestureRecognizer;
@end


/// 与 QMUINavigationController push/pop 相关的一些方法
@protocol QMUINavigationControllerTransitionDelegate <NSObject>

@optional

/**
 *  当前界面正处于手势返回的过程中，可自行通过 gestureRecognizer.state 来区分手势返回的各个阶段。手势返回有多个阶段（手势返回开始、拖拽过程中、松手并成功返回、松手但不切换界面），不同阶段的 viewController 的状态可能不一样。
 *  @param navigationController 当前正在手势返回的 QMUINavigationController，由于某些阶段下无法通过 vc.navigationController 获取到 nav 的引用，所以直接传一个参数
 *  @param gestureRecognizer 手势对象
 *  @param viewControllerWillDisappear 手势返回中顶部的那个 vc
 *  @param viewControllerWillAppear 手势返回中背后的那个 vc
 */
- (void)navigationController:(nonnull QMUINavigationController *)navigationController poppingByInteractiveGestureRecognizer:(nullable UIScreenEdgePanGestureRecognizer *)gestureRecognizer viewControllerWillDisappear:(nullable UIViewController *)viewControllerWillDisappear viewControllerWillAppear:(nullable UIViewController *)viewControllerWillAppear;

/**
 *  在 self.navigationController 进行以下 4 个操作前，相应的 viewController 的 willPopInNavigationControllerWithAnimated: 方法会被调用：
 *  1. popViewControllerAnimated:
 *  2. popToViewController:animated:
 *  3. popToRootViewControllerAnimated:
 *  4. setViewControllers:animated:
 *
 *  此时 self 仍存在于 self.navigationController.viewControllers 堆栈内。
 *
 *  在 ARC 环境下，viewController 可能被放在 autorelease 池中，因此 viewController 被pop后不一定立即被销毁，所以一些对实时性要求很高的内存管理逻辑可以写在这里（而不是写在dealloc内）
 *
 *  @warning 不要尝试将 willPopInNavigationControllerWithAnimated: 视为点击返回按钮的回调，因为导致 viewController 被 pop 的情况不止点击返回按钮这一途径。系统的返回按钮是无法添加回调的，只能使用自定义的返回按钮。
 */
- (void)willPopInNavigationControllerWithAnimated:(BOOL)animated;

/**
 *  在 self.navigationController 进行以下 4 个操作后，相应的 viewController 的 didPopInNavigationControllerWithAnimated: 方法会被调用：
 *  1. popViewControllerAnimated:
 *  2. popToViewController:animated:
 *  3. popToRootViewControllerAnimated:
 *  4. setViewControllers:animated:
 *
 *  @warning 此时 self 已经不在 viewControllers 数组内
 */
- (void)didPopInNavigationControllerWithAnimated:(BOOL)animated;

/**
 *  当通过 setViewControllers:animated: 来修改 viewController 的堆栈时，如果参数 viewControllers.lastObject 与当前的 self.viewControllers.lastObject 不相同，则意味着会产生界面的切换，这种情况系统会自动调用两个切换的界面的生命周期方法，但如果两者相同，则意味着并不会产生界面切换，此时之前就已经在显示的那个 viewController 的 viewWillAppear:、viewDidAppear: 并不会被调用，那如果用户确实需要在这个时候修改一些界面元素，则找不到一个时机。所以这个方法就是提供这样一个时机给用户修改界面元素。
 */
- (void)viewControllerKeepingAppearWhenSetViewControllersWithAnimated:(BOOL)animated;

@end


/// 与 QMUINavigationController 外观样式相关的方法
@protocol QMUINavigationControllerAppearanceDelegate <NSObject>

@optional

/// 是否需要将状态栏改为浅色文字，对于 QMUICommonViewController 子类，返回值默认为宏 StatusbarStyleLightInitially 的值，对于 UIViewController，不实现该方法则视为返回 NO。
/// @warning 需在项目的 Info.plist 文件内设置字段 “View controller-based status bar appearance” 的值为 NO 才能生效，如果不设置，或者值为 YES，则请使用系统提供的 - preferredStatusBarStyle 方法
- (BOOL)shouldSetStatusBarStyleLight;

/// 设置 titleView 的 tintColor
- (nullable UIColor *)titleViewTintColor;

/// 设置导航栏的背景图，默认为 NavBarBackgroundImage
- (nullable UIImage *)navigationBarBackgroundImage;

/// 设置导航栏底部的分隔线图片，默认为 NavBarShadowImage，必须在 navigationBar 设置了背景图后才有效（系统限制如此）
- (nullable UIImage *)navigationBarShadowImage;

/// 设置当前导航栏的 barTintColor，默认为 NavBarBarTintColor
- (nullable UIColor *)navigationBarBarTintColor;

/// 设置当前导航栏的 UIBarButtonItem 的 tintColor，默认为NavBarTintColor
- (nullable UIColor *)navigationBarTintColor;

/// 设置系统返回按钮title，如果返回nil则使用系统默认的返回按钮标题
- (nullable NSString *)backBarButtonItemTitleWithPreviousViewController:(nullable UIViewController *)viewController;

@end


/// 与 QMUINavigationController 控制 navigationBar 显隐/动画相关的方法
@protocol QMUICustomNavigationBarTransitionDelegate <NSObject>

@optional

/// 设置每个界面导航栏的显示/隐藏，为了减少对项目的侵入性，默认不开启这个接口的功能，只有当 shouldCustomizeNavigationBarTransitionIfHideable 返回 YES 时才会开启此功能。如果需要全局开启，那么就在 Controller 基类里面返回 YES；如果是老项目并不想全局使用此功能，那么则可以在单独的界面里面开启。
- (BOOL)preferredNavigationBarHidden;

/**
 *  当切换界面时，如果不同界面导航栏的显隐状态不同，可以通过 shouldCustomizeNavigationBarTransitionIfHideable 设置是否需要接管导航栏的显示和隐藏。从而不需要在各自的界面的 viewWillAppear 和 viewWillDisappear 里面去管理导航栏的状态。
 *  @see UINavigationController+NavigationBarTransition.h
 *  @see preferredNavigationBarHidden
 */
- (BOOL)shouldCustomizeNavigationBarTransitionIfHideable;

/**
 *  设置当前导航栏是否需要使用自定义的 push/pop transition 效果，默认返回NO。<br/>
 *  因为系统的UINavigationController只有一个navBar，所以会导致在切换controller的时候，如果两个controller的navBar状态不一致（包括backgroundImage、shadowImage、barTintColor等等），就会导致在刚要切换的瞬间，navBar的状态都立马变成下一个controller所设置的样式了，为了解决这种情况，QMUI给出了一个方案，有四个方法可以决定你在转场的时候要不要使用自定义的navBar来模仿真实的navBar。具体方法如下：
 *  @see UINavigationController+NavigationBarTransition.h
 */
- (BOOL)shouldCustomNavigationBarTransitionWhenPushAppearing;

/**
 *  同上
 *  @see UINavigationController+NavigationBarTransition.h
 */
- (BOOL)shouldCustomNavigationBarTransitionWhenPushDisappearing;

/**
 *  同上
 *  @see UINavigationController+NavigationBarTransition.h
 */
- (BOOL)shouldCustomNavigationBarTransitionWhenPopAppearing;

/**
 *  同上
 *  @see UINavigationController+NavigationBarTransition.h
 */
- (BOOL)shouldCustomNavigationBarTransitionWhenPopDisappearing;

/**
 *  自定义navBar效果过程中UINavigationController的containerView的背景色
 *  @see UINavigationController+NavigationBarTransition.h
 */
- (nullable UIColor *)containerViewBackgroundColorWhenTransitioning;

@end


/**
 *  配合 QMUINavigationController 使用，当 navController 里的 UIViewController 实现了这个协议时，则可得到协议里各个方法的功能。
 *  QMUICommonViewController、QMUICommonTableViewController 默认实现了这个协议，所以子类无需再手动实现一遍。
 */
@protocol QMUINavigationControllerDelegate <UINavigationControllerDelegate, QMUINavigationControllerTransitionDelegate, QMUINavigationControllerAppearanceDelegate, QMUICustomNavigationBarTransitionDelegate>

@end
